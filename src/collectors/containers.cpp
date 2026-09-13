#include "containers.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void ContainerCollector::collect() {
    // Skeleton: Docker Unix socket client
    MetricRegistry::instance().update_metric("sysmon_docker_containers_running", 3.0);
}

} // namespace sysmon
