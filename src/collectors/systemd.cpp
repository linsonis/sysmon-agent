#include "systemd.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void SystemdCollector::collect() {
    // Skeleton: inspect systemd units or /run/systemd
    MetricRegistry::instance().update_metric("sysmon_systemd_failed_units", 0.0);
}

} // namespace sysmon
