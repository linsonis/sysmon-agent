#pragma once

#include <string>
#include <unordered_map>
#include <map>

namespace sysmon {

enum class MetricType {
    COUNTER,
    GAUGE,
    HISTOGRAM
};

struct Metric {
    std::string name;
    std::string help;
    MetricType type = MetricType::GAUGE;
    double value = 0.0;
    std::unordered_map<std::string, std::string> labels;

    // Generates a unique key based on name and sorted labels
    std::string key() const {
        std::string k = name;
        if (!labels.empty()) {
            std::map<std::string, std::string> sorted_labels(labels.begin(), labels.end());
            k += "{";
            bool first = true;
            for (const auto& [lbl_key, lbl_val] : sorted_labels) {
                if (!first) k += ",";
                k += lbl_key + "=\"" + lbl_val + "\"";
                first = false;
            }
            k += "}";
        }
        return k;
    }
};

} // namespace sysmon
