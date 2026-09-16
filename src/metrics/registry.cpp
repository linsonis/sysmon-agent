#include "registry.hpp"

namespace sysmon {

MetricRegistry& MetricRegistry::instance() {
    static MetricRegistry inst;
    return inst;
}

void MetricRegistry::register_metric(const Metric& metric) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_map_[metric.key()] = metric;
}

void MetricRegistry::update_metric(const std::string& name,
                                   double value,
                                   const std::unordered_map<std::string, std::string>& labels,
                                   MetricType type,
                                   const std::string& help) {
    std::lock_guard<std::mutex> lock(mutex_);
    Metric m{name, help, type, value, labels};
    std::string k = m.key();
    
    auto it = metrics_map_.find(k);
    if (it != metrics_map_.end()) {
        it->second.value = value;
        if (!help.empty()) {
            it->second.help = help;
        }
    } else {
        metrics_map_[k] = m;
    }
}

void MetricRegistry::increment_counter(const std::string& name,
                                       double amount,
                                       const std::unordered_map<std::string, std::string>& labels,
                                       const std::string& help) {
    std::lock_guard<std::mutex> lock(mutex_);
    Metric dummy{name, help, MetricType::COUNTER, 0.0, labels};
    std::string k = dummy.key();

    auto it = metrics_map_.find(k);
    if (it != metrics_map_.end()) {
        it->second.value += amount;
        if (!help.empty()) {
            it->second.help = help;
        }
    } else {
        metrics_map_[k] = Metric{name, help, MetricType::COUNTER, amount, labels};
    }
}

std::vector<Metric> MetricRegistry::get_metrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Metric> result;
    result.reserve(metrics_map_.size());
    for (const auto& [key, metric] : metrics_map_) {
        result.push_back(metric);
    }
    return result;
}

void MetricRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_map_.clear();
}

} // namespace sysmon
