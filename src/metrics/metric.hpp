#pragma once

#include <string>
#include <unordered_map>

namespace sysmon {

enum class MetricType {
    COUNTER,
    GAUGE,
    HISTOGRAM
};

struct Metric {
    std::string name;
    std::string help;
    MetricType type;
    double value = 0.0;
    std::unordered_map<std::string, std::string> labels;
};

} // namespace sysmon
