#include "common/config.hpp"
#include "common/logging.hpp"
#include "http/server.hpp"
#include "scheduler/scheduler.hpp"
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
        
        clear();
        mvprintw(0, 2, "=== SYSMON TUI DASHBOARD (Press 'q' to quit) ===");
        mvprintw(2, 2, "CPU: Active | Memory: Active | Disk: Active | Containers: Active");
        mvprintw(4, 2, "HTTP Prometheus exporter running on port 9100");
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
