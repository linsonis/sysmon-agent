#include "disk.hpp"
#include "../metrics/registry.hpp"
#include <fstream>
#include <sstream>
#include <string>

namespace sysmon {

void DiskCollector::collect() {
    std::ifstream file("/proc/diskstats");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        unsigned int major = 0, minor = 0;
        std::string device;
        
        unsigned long long reads_completed = 0;
        unsigned long long reads_merged = 0;
        unsigned long long sectors_read = 0;
        unsigned long long read_time_ms = 0;
        
        unsigned long long writes_completed = 0;
        unsigned long long writes_merged = 0;
        unsigned long long sectors_written = 0;
        unsigned long long write_time_ms = 0;
        
        unsigned long long io_in_progress = 0;
        unsigned long long io_time_ms = 0;
        unsigned long long weighted_io_time_ms = 0;

        if (ss >> major >> minor >> device
               >> reads_completed >> reads_merged >> sectors_read >> read_time_ms
               >> writes_completed >> writes_merged >> sectors_written >> write_time_ms
               >> io_in_progress >> io_time_ms >> weighted_io_time_ms) {
            
            // Skip loop and ram devices to avoid noise
            if (device.rfind("loop", 0) == 0 || device.rfind("ram", 0) == 0) {
                continue;
            }

            double read_bytes = static_cast<double>(sectors_read) * 512.0;
            double write_bytes = static_cast<double>(sectors_written) * 512.0;

            MetricRegistry::instance().update_metric("sysmon_disk_reads_completed_total", static_cast<double>(reads_completed), {{"device", device}});
            MetricRegistry::instance().update_metric("sysmon_disk_read_bytes_total", read_bytes, {{"device", device}});
            MetricRegistry::instance().update_metric("sysmon_disk_read_time_ms_total", static_cast<double>(read_time_ms), {{"device", device}});

            MetricRegistry::instance().update_metric("sysmon_disk_writes_completed_total", static_cast<double>(writes_completed), {{"device", device}});
            MetricRegistry::instance().update_metric("sysmon_disk_write_bytes_total", write_bytes, {{"device", device}});
            MetricRegistry::instance().update_metric("sysmon_disk_write_time_ms_total", static_cast<double>(write_time_ms), {{"device", device}});

            MetricRegistry::instance().update_metric("sysmon_disk_io_time_ms_total", static_cast<double>(io_time_ms), {{"device", device}});
        }
    }
}

} // namespace sysmon
