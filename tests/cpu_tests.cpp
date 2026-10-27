#include <gtest/gtest.h>
#include "collectors/cpu.hpp"
#include "metrics/registry.hpp"
#include <chrono>
#include <thread>
#include <vector>

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

static double cpu_ratio() {
    for (const auto& m : sysmon::MetricRegistry::instance().get_metrics()) {
        if (m.name == "sysmon_cpu_usage_ratio") return m.value;
    }
    return -1.0;
}

TEST(CpuCollectorTest, IgnoresSamplesInsideMinWindow) {
    sysmon::CpuCollector::collect();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    sysmon::CpuCollector::collect();  // opens a fresh window

    // Back-to-back calls (e.g. a held key) must not overwrite the last reading.
    sysmon::MetricRegistry::instance().update_metric("sysmon_cpu_usage_ratio", 0.777);
    for (int i = 0; i < 50; ++i) sysmon::CpuCollector::collect();
    EXPECT_DOUBLE_EQ(cpu_ratio(), 0.777);

    // Once the window has elapsed, a new reading replaces it.
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    sysmon::CpuCollector::collect();
    double r = cpu_ratio();
    EXPECT_NE(r, 0.777);
    EXPECT_GE(r, 0.0);
    EXPECT_LE(r, 1.0);
}

TEST(CpuCollectorTest, ConcurrentCollectIsSafe) {
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([]() {
            for (int i = 0; i < 200; ++i) sysmon::CpuCollector::collect();
        });
    }
    for (auto& th : threads) th.join();
    double r = cpu_ratio();
    EXPECT_GE(r, 0.0);
    EXPECT_LE(r, 1.0);
}
