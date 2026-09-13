#include "filesystem.hpp"
#include "../metrics/registry.hpp"

namespace sysmon {

void FilesystemCollector::collect() {
    // Skeleton: statvfs for mounted filesystems
    MetricRegistry::instance().update_metric("sysmon_filesystem_free_bytes", 50000000000UL, {{"mount", "/"}});
}

} // namespace sysmon
