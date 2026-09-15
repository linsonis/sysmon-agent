#include "systemd.hpp"
#include "../metrics/registry.hpp"
#include <cstdio>
#include <memory>
#include <array>
#include <string>
#include <sstream>

namespace sysmon {

void SystemdCollector::collect() {
    double active_units = 0;
    double failed_units = 0;
    double total_units = 0;

    // Run systemctl command to list unit states
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("systemctl list-units --type=service --no-legend --plain 2>/dev/null", "r"),
        pclose
    );

    if (pipe) {
        std::array<char, 256> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            std::string line(buffer.data());
            std::istringstream ss(line);
            std::string unit_name, load_state, active_state, sub_state;

            if (ss >> unit_name >> load_state >> active_state >> sub_state) {
                total_units++;
                if (active_state == "active") {
                    active_units++;
                } else if (active_state == "failed") {
                    failed_units++;
                    // Export specific failed unit metric
                    MetricRegistry::instance().update_metric("sysmon_systemd_unit_failed", 1.0, {{"name", unit_name}});
                }
            }
        }
    }

    MetricRegistry::instance().update_metric("sysmon_systemd_units_total", total_units);
    MetricRegistry::instance().update_metric("sysmon_systemd_active_units", active_units);
    MetricRegistry::instance().update_metric("sysmon_systemd_failed_units", failed_units);
}

} // namespace sysmon
