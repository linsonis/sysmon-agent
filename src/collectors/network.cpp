#include "network.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void NetworkCollector::collect() {
    // Skeleton: parse /proc/net/dev
    MetricRegistry::instance().update_metric("sysmon_network_receive_bytes_total", 1024500.0, {{"interface", "eth0"}});
}

} // namespace sysmon
