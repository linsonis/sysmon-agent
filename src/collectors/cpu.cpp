#include "cpu.hpp"
#include "../metrics/registry.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>

namespace sysmon {

struct CpuState {
    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    unsigned long long steal = 0;

    unsigned long long get_total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }

    unsigned long long get_idle() const {
        return idle + iowait;
    }
};

// Map to store previous states: key is "cpu" (aggregate) or "cpu0", "cpu1", etc.
static std::unordered_map<std::string, CpuState> prev_cpu_states;

void CpuCollector::collect() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("cpu", 0) != 0) {
            // Lines starting with "cpu" are at the top, stop or skip others
            break;
        }

        std::istringstream ss(line);
        std::string cpu_name;
        ss >> cpu_name;

        CpuState current;
        if (ss >> current.user >> current.nice >> current.system >> current.idle
               >> current.iowait >> current.irq >> current.softirq >> current.steal) {
            
            auto it = prev_cpu_states.find(cpu_name);
            if (it != prev_cpu_states.end()) {
                const auto& prev = it->second;
                double total_diff = current.get_total() - prev.get_total();
                double idle_diff = current.get_idle() - prev.get_idle();

                double usage = 0.0;
                if (total_diff > 0.0) {
                    usage = 1.0 - (idle_diff / total_diff);
                    if (usage < 0.0) usage = 0.0;
                    if (usage > 1.0) usage = 1.0;
                }

                if (cpu_name == "cpu") {
                    MetricRegistry::instance().update_metric("sysmon_cpu_usage_ratio", usage);
                } else {
                    MetricRegistry::instance().update_metric("sysmon_cpu_core_usage_ratio", usage, {{"core", cpu_name.substr(3)}});
                }
            } else {
                // First reading, register with initial usage (or 0)
                if (cpu_name == "cpu") {
                    MetricRegistry::instance().update_metric("sysmon_cpu_usage_ratio", 0.0);
                } else {
                    MetricRegistry::instance().update_metric("sysmon_cpu_core_usage_ratio", 0.0, {{"core", cpu_name.substr(3)}});
                }
            }
            prev_cpu_states[cpu_name] = current;
        }
    }
}

} // namespace sysmon
