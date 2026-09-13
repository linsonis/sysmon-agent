#include "memory.hpp"
#include "../metrics/registry.hpp"
#include <fstream>

namespace sysmon {

void MemoryCollector::collect() {
    // Skeleton: parse /proc/meminfo
    MetricRegistry::instance().update_metric("sysmon_memory_used_bytes", 4096UL * 1024 * 1024);
    MetricRegistry::instance().update_metric("sysmon_memory_total_bytes", 16384UL * 1024 * 1024);
}

} // namespace sysmon
