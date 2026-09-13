#include "registry.hpp"

namespace sysmon {

MetricRegistry& MetricRegistry::instance() {
    static MetricRegistry inst;
    return inst;
}

void MetricRegistry::register_metric(const Metric& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.push_back(metric);
}

void MetricRegistry::update_metric(const std::string& name, double value, const std::unordered_map<std::string, std::string>& labels) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& m : metrics_) {
        if (m.name == name) {
            m.value = value;
            m.labels = labels;
            return;
        }
    }
    // If not found, register new
    metrics_.push_back({name, "", MetricType::GAUGE, value, labels});
}

std::vector<Metric> MetricRegistry::get_metrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_;
}

} // namespace sysmon
