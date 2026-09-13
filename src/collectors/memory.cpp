#include "memory.hpp"
#include "../metrics/registry.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace sysmon {

void MemoryCollector::collect() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        return;
    }

    std::unordered_map<std::string, unsigned long long> mem_info;
    std::string line;
    while (std::getline(file, line)) {
        std::string key;
        unsigned long long val = 0;
        std::string unit;
        
        std::size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            continue;
        }

        key = line.substr(0, colon_pos);
        std::istringstream ss(line.substr(colon_pos + 1));
        if (ss >> val) {
            // Usually kB, convert to bytes if it's in kB
            ss >> unit;
            if (unit == "kB") {
                val *= 1024;
            }
            mem_info[key] = val;
        }
    }

    unsigned long long total = mem_info["MemTotal"];
    unsigned long long free = mem_info["MemFree"];
    unsigned long long buffers = mem_info["Buffers"];
    unsigned long long cached = mem_info["Cached"];
    unsigned long long reclaimable = mem_info["SReclaimable"]; // often included in freeable cache
    unsigned long long available = 0;

    auto it = mem_info.find("MemAvailable");
    if (it != mem_info.end()) {
        available = it->second;
    } else {
        available = free + buffers + cached + reclaimable;
    }

    unsigned long long used = (total > available) ? (total - available) : 0;
    unsigned long long swap_total = mem_info["SwapTotal"];
    unsigned long long swap_free = mem_info["SwapFree"];
    unsigned long long swap_used = (swap_total > swap_free) ? (swap_total - swap_free) : 0;

    MetricRegistry::instance().update_metric("sysmon_memory_total_bytes", static_cast<double>(total));
    MetricRegistry::instance().update_metric("sysmon_memory_free_bytes", static_cast<double>(free));
    MetricRegistry::instance().update_metric("sysmon_memory_available_bytes", static_cast<double>(available));
    MetricRegistry::instance().update_metric("sysmon_memory_used_bytes", static_cast<double>(used));
    MetricRegistry::instance().update_metric("sysmon_memory_swap_total_bytes", static_cast<double>(swap_total));
    MetricRegistry::instance().update_metric("sysmon_memory_swap_used_bytes", static_cast<double>(swap_used));
}

} // namespace sysmon
