#include "common/config.hpp"
#include "common/logging.hpp"
#include "http/server.hpp"
#include "scheduler/scheduler.hpp"
#include "metrics/registry.hpp"
#include "collectors/cpu.hpp"
#include "collectors/memory.hpp"
#include "collectors/process.hpp"
#include "collectors/network.hpp"
#include "collectors/disk.hpp"
#include "collectors/filesystem.hpp"
#include "collectors/systemd.hpp"
#include "collectors/containers.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

#ifdef __has_include
#if __has_include(<ncurses.h>)
#include <ncurses.h>
#define HAS_NCURSES 1
#endif
#endif

void run_collectors() {
    sysmon::CpuCollector::collect();
    sysmon::MemoryCollector::collect();
    sysmon::ProcessCollector::collect();
    sysmon::NetworkCollector::collect();
    sysmon::DiskCollector::collect();
    sysmon::FilesystemCollector::collect();
    sysmon::SystemdCollector::collect();
    sysmon::ContainerCollector::collect();
}

void run_tui() {
#ifdef HAS_NCURSES
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(1000);

    bool running = true;
    while (running) {
        run_collectors();

        auto metrics = sysmon::MetricRegistry::instance().get_metrics();
        double cpu_usage = 0.0;
        double mem_used = 0.0;
        double mem_total = 0.0;
        double process_count = 0.0;
        double failed_units = 0.0;
        double containers_running = 0.0;

        for (const auto& m : metrics) {
            if (m.name == "sysmon_cpu_usage_ratio") cpu_usage = m.value * 100.0;
            else if (m.name == "sysmon_memory_used_bytes") mem_used = m.value / (1024.0 * 1024.0);
            else if (m.name == "sysmon_memory_total_bytes") mem_total = m.value / (1024.0 * 1024.0);
            else if (m.name == "sysmon_process_count") process_count = m.value;
            else if (m.name == "sysmon_systemd_failed_units") failed_units = m.value;
            else if (m.name == "sysmon_docker_containers_running") containers_running = m.value;
        }

        clear();
        mvprintw(0, 2, "==================================================");
        mvprintw(1, 2, "    System Monitor         (Press 'q' to quit)");
        mvprintw(2, 2, "==================================================");

        mvprintw(4, 2, "CPU Usage        : [%-20s] %.1f%%",
                 std::string(static_cast<int>(cpu_usage / 5.0), '#').c_str(), cpu_usage);
        mvprintw(5, 2, "Memory Used      : %.1f MB / %.1f MB (%.1f%%)",
                 mem_used, mem_total, (mem_total > 0 ? (mem_used / mem_total) * 100.0 : 0.0));
        mvprintw(6, 2, "Active Processes : %.0f", process_count);
        mvprintw(7, 2, "Systemd Failed   : %.0f units", failed_units);
        mvprintw(8, 2, "Docker Containers: %.0f running", containers_running);

        mvprintw(11, 2, "Prometheus HTTP Exporter: http://0.0.0.0:9100/metrics");
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
        }
    }

    endwin();
#else
    std::cout << "NCurses not available. Running headless collection loop...\n";
    while (true) {
        run_collectors();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#endif
}

int main(int argc, char* argv[]) {
    sysmon::Config config;
    config.load_from_file("deploy/config.yaml");

    bool service_mode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--service") {
            service_mode = true;
        }
    }

    sysmon::Logger::instance().info("Starting sysmon agent...");

    // Start metrics collection scheduler
    sysmon::Scheduler scheduler;
    scheduler.start(std::chrono::milliseconds(config.collection_interval_ms), run_collectors);

    // Start HTTP server for Prometheus
    sysmon::HttpServer server;
    server.start(config.http_port);

    if (service_mode) {
        std::cout << "Sysmon running as service on port " << config.http_port << "...\n";
        // Block main thread
        while (true) {
            std::this_thread::sleep_for(std::chrono::hours(24));
        }
    } else {
        run_tui();
    }

    scheduler.stop();
    server.stop();
    return 0;
}
