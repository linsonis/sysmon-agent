#pragma once

#include "metric.hpp"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

namespace sysmon {

class MetricRegistry {
public:
    static MetricRegistry& instance();
    
    void update_metric(const std::string& name,
                       double value,
                       const std::unordered_map<std::string, std::string>& labels = {},
                       MetricType type = MetricType::GAUGE,
                       const std::string& help = "");

    void increment_counter(const std::string& name,
                           double amount = 1.0,
                           const std::unordered_map<std::string, std::string>& labels = {},
                           const std::string& help = "");
                        
    void register_metric(const Metric& metric);
    std::vector<Metric> get_metrics() const;
    void clear();

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Metric> metrics_map_;
};

} // namespace sysmon
