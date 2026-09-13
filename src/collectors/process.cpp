#include "process.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void ProcessCollector::collect() {
    // Skeleton: parse /proc/[pid]/stat
    MetricRegistry::instance().update_metric("sysmon_process_count", 150.0);
}

} // namespace sysmon
