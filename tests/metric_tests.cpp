#include <gtest/GTest.h>
#include "metrics/registry.hpp"
#include "metrics/prometheus.hpp"

TEST(MetricRegistryTest, UpdateAndRender) {
    sysmon::MetricRegistry::instance().clear();
    sysmon::MetricRegistry::instance().update_metric("test_metric_total", 42.0, {}, sysmon::MetricType::GAUGE, "A test gauge metric");
    std::string output = sysmon::PrometheusExporter::render();
    EXPECT_NE(output.find("test_metric_total 42"), std::string::npos);
    EXPECT_NE(output.find("# TYPE test_metric_total gauge"), std::string::npos);
}

TEST(MetricRegistryTest, IncrementCounter) {
    sysmon::MetricRegistry::instance().clear();
    sysmon::MetricRegistry::instance().increment_counter("sysmon_requests_total", 1.0, {{"method", "GET"}}, "Total requests");
    sysmon::MetricRegistry::instance().increment_counter("sysmon_requests_total", 2.5, {{"method", "GET"}});
    
    auto metrics = sysmon::MetricRegistry::instance().get_metrics();
    bool found = false;
    for (const auto& m : metrics) {
        if (m.name == "sysmon_requests_total" && m.value == 3.5) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST(PrometheusExporterTest, LabelSortingAndFormatting) {
    sysmon::MetricRegistry::instance().clear();
    sysmon::MetricRegistry::instance().update_metric(
        "sysmon_disk_bytes",
        1024.0,
        {{"device", "sda"}, {"mount", "/"}},
        sysmon::MetricType::GAUGE,
        "Disk usage bytes"
    );

    std::string output = sysmon::PrometheusExporter::render();
    EXPECT_NE(output.find("sysmon_disk_bytes{device=\"sda\",mount=\"/\"} 1024"), std::string::npos);
}
