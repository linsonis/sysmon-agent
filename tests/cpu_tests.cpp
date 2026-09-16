#include <gtest/gtest.h>
#include "collectors/cpu.hpp"
#include "metrics/registry.hpp"

TEST(CpuCollectorTest, CollectsMetric) {
    sysmon::CpuCollector::collect();
    auto metrics = sysmon::MetricRegistry::instance().get_metrics();
    bool found = false;
    for (const auto& m : metrics) {
        if (m.name == "sysmon_cpu_usage_ratio") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}
