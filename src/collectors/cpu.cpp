#include "cpu.hpp"
#include "../metrics/registry.hpp"
#include <fstream>
#include <sstream>

namespace sysmon {

void CpuCollector::collect() {
    // Skeleton: parse /proc/stat
    MetricRegistry::instance().update_metric("sysmon_cpu_usage_ratio", 0.15);
}

} // namespace sysmon
