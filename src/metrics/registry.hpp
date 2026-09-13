#pragma once

#include "metric.hpp"
#include <vector>
#include <mutex>
#include <string>

namespace sysmon {

class MetricRegistry {
public:
    static MetricRegistry& instance();
    void register_metric(const Metric& metric);
    void update_metric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& labels = {});
    std::vector<Metric> get_metrics() const;

private:
    mutable std::mutex mutex_;
    std::vector<Metric> metrics_;
};

} // namespace sysmon
