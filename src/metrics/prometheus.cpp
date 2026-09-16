#include "prometheus.hpp"
#include "registry.hpp"
#include <sstream>
#include <map>
#include <vector>

namespace sysmon {

std::string PrometheusExporter::render() {
    auto metrics = MetricRegistry::instance().get_metrics();
    
    // Group metrics by family name
    std::map<std::string, std::vector<Metric>> families;
    for (const auto& m : metrics) {
        families[m.name].push_back(m);
    }

    std::ostringstream ss;
    for (const auto& [name, samples] : families) {
        if (samples.empty()) continue;

        const auto& first = samples.front();
        std::string help = first.help.empty() ? (name + " metric") : first.help;

        ss << "# HELP " << name << " " << help << "\n";
        ss << "# TYPE " << name << " ";
        switch (first.type) {
            case MetricType::COUNTER: ss << "counter"; break;
            case MetricType::GAUGE: ss << "gauge"; break;
            case MetricType::HISTOGRAM: ss << "histogram"; break;
        }
        ss << "\n";

        for (const auto& m : samples) {
            ss << m.name;
            if (!m.labels.empty()) {
                ss << "{";
                // Sort labels for deterministic output format
                std::map<std::string, std::string> sorted_labels(m.labels.begin(), m.labels.end());
                bool first_label = true;
                for (const auto& [k, v] : sorted_labels) {
                    if (!first_label) ss << ",";
                    ss << k << "=\"" << v << "\"";
                    first_label = false;
                }
                ss << "}";
            }
            ss << " " << m.value << "\n";
        }
        ss << "\n";
    }
    return ss.str();
}

} // namespace sysmon
