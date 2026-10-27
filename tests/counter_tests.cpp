#include <gtest/gtest.h>
#include "collectors/disk.hpp"
#include "collectors/network.hpp"
#include "metrics/prometheus.hpp"
#include <map>
#include <sstream>
#include <string>

// Parse "# TYPE <name> <type>" lines from the exporter output.
static std::map<std::string, std::string> exported_types() {
    std::istringstream in(sysmon::PrometheusExporter::render());
    std::map<std::string, std::string> types;
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("# TYPE ", 0) != 0) continue;
        std::istringstream ls(line.substr(7));
        std::string name, type;
        ls >> name >> type;
        types[name] = type;
    }
    return types;
}

// Cumulative /proc counters must be exported as counters so Prometheus
// tooling (rate(), reset handling, type-aware UIs) treats them correctly.
TEST(CounterTypeTest, NetworkAndDiskTotalsAreCounters) {
    sysmon::NetworkCollector::collect();
    sysmon::DiskCollector::collect();

    int checked = 0;
    for (const auto& [name, type] : exported_types()) {
        bool cumulative = (name.rfind("sysmon_network_", 0) == 0 || name.rfind("sysmon_disk_", 0) == 0)
                          && name.size() > 6 && name.compare(name.size() - 6, 6, "_total") == 0;
        if (!cumulative) continue;
        EXPECT_EQ(type, "counter") << name;
        ++checked;
    }
    EXPECT_GT(checked, 0) << "no network/disk *_total metrics were exported";
}
