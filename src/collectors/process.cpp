#include "process.hpp"
#include "../metrics/registry.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

namespace sysmon {

void ProcessCollector::collect() {
    namespace fs = std::filesystem;
    if (!fs::exists("/proc")) {
        return;
    }

    double total_processes = 0;
    double running_count = 0;
    double sleeping_count = 0;
    double zombie_count = 0;
    double stopped_count = 0;

    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            if (!entry.is_directory()) {
                continue;
            }

            std::string dirname = entry.path().filename().string();
            if (!std::all_of(dirname.begin(), dirname.end(), ::isdigit)) {
                continue;
            }

            total_processes++;

            // Read /proc/[pid]/stat for process state
            std::string stat_path = entry.path().string() + "/stat";
            std::ifstream file(stat_path);
            if (!file.is_open()) {
                continue;
            }

            std::string line;
            if (std::getline(file, line)) {
                // The filename (comm) can contain spaces and parentheses, e.g. (my process name)
                // We find the last closing parenthesis to locate the end of field 2
                std::size_t r_paren = line.rfind(')');
                if (r_paren != std::string::npos && r_paren + 2 < line.length()) {
                    char state = line[r_paren + 2];
                    switch (state) {
                        case 'R': running_count++; break;
                        case 'S':
                        case 'D': sleeping_count++; break;
                        case 'Z': zombie_count++; break;
                        case 'T': stopped_count++; break;
                        default: break;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // Handle gracefully, directory iterator could throw if file disappears
    }

    MetricRegistry::instance().update_metric("sysmon_process_count", total_processes);
    MetricRegistry::instance().update_metric("sysmon_process_state_count", running_count, {{"state", "running"}});
    MetricRegistry::instance().update_metric("sysmon_process_state_count", sleeping_count, {{"state", "sleeping"}});
    MetricRegistry::instance().update_metric("sysmon_process_state_count", zombie_count, {{"state", "zombie"}});
    MetricRegistry::instance().update_metric("sysmon_process_state_count", stopped_count, {{"state", "stopped"}});
}

} // namespace sysmon
