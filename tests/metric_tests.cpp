#include <gtest/GTest.h>
#include "metrics/registry.hpp"
#include "metrics/prometheus.hpp"

TEST(MetricRegistryTest, UpdateAndRender) {
    sysmon::MetricRegistry::instance().update_metric("test_metric_total", 42.0);
    std::string output = sysmon::PrometheusExporter::render();
    EXPECT_NE(output.find("test_metric_total 42"), std::string::npos);
}
