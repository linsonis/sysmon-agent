#include <gtest/GTest.h>
#include "collectors/memory.hpp"
#include "metrics/registry.hpp"

TEST(MemoryCollectorTest, CollectsMemoryMetrics) {
    sysmon::MemoryCollector::collect();
    auto metrics = sysmon::MetricRegistry::instance().get_metrics();
    bool found = false;
    for (const auto& m : metrics) {
        if (m.name == "sysmon_memory_used_bytes") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}
