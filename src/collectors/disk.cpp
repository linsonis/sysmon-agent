#include "disk.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void DiskCollector::collect() {
    // Skeleton: parse /proc/diskstats
    MetricRegistry::instance().update_metric("sysmon_disk_read_bytes_total", 500000.0, {{"device", "sda"}});
}

} // namespace sysmon
