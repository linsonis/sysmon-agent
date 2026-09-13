#include "prometheus.hpp"
#include "registry.hpp"
#include <sstream>

namespace sysmon {

std::string PrometheusExporter::render() {
    auto metrics = MetricRegistry::instance().get_metrics();
    std::ostringstream ss;
    for (const auto& m : metrics) {
        ss << "# HELP " << m.name << " " << m.help << "\n";
        ss << "# TYPE " << m.name << " ";
        switch (m.type) {
            case MetricType::COUNTER: ss << "counter"; break;
            case MetricType::GAUGE: ss << "gauge"; break;
            case MetricType::HISTOGRAM: ss << "histogram"; break;
        }
        ss << "\n";
        ss << m.name;
        if (!m.labels.empty()) {
            ss << "{";
            bool first = true;
            for (const auto& [k, v] : m.labels) {
                if (!first) ss << ",";
                ss << k << "=\"" << v << "\"";
                first = false;
            }
            ss << "}";
        }
        ss << " " << m.value << "\n";
    }
    return ss.str();
}

} // namespace sysmon
