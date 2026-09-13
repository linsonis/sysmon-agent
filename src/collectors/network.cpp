#include "network.hpp"
#include "../metrics/registry.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

namespace sysmon {

void NetworkCollector::collect() {
    std::ifstream file("/proc/net/dev");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    // Skip the first two header lines
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            continue;
        }

        std::string interface = line.substr(0, colon_pos);
        // Trim leading and trailing spaces from the interface name
        interface.erase(interface.begin(), std::find_if(interface.begin(), interface.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        interface.erase(std::find_if(interface.rbegin(), interface.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), interface.end());

        if (interface.empty()) {
            continue;
        }

        std::istringstream ss(line.substr(colon_pos + 1));
        unsigned long long rx_bytes = 0, rx_packets = 0, rx_errs = 0, rx_drop = 0;
        unsigned long long dummy = 0; // for other receive columns
        unsigned long long tx_bytes = 0, tx_packets = 0, tx_errs = 0, tx_drop = 0;

        // Parse receive columns: bytes, packets, errs, drop, fifo, frame, compressed, multicast
        if (ss >> rx_bytes >> rx_packets >> rx_errs >> rx_drop >> dummy >> dummy >> dummy >> dummy) {
            // Parse transmit columns: bytes, packets, errs, drop, fifo, colls, carrier, compressed
            if (ss >> tx_bytes >> tx_packets >> tx_errs >> tx_drop) {
                MetricRegistry::instance().update_metric("sysmon_network_receive_bytes_total", static_cast<double>(rx_bytes), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_receive_packets_total", static_cast<double>(rx_packets), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_receive_errors_total", static_cast<double>(rx_errs), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_receive_drop_total", static_cast<double>(rx_drop), {{"interface", interface}});

                MetricRegistry::instance().update_metric("sysmon_network_transmit_bytes_total", static_cast<double>(tx_bytes), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_transmit_packets_total", static_cast<double>(tx_packets), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_transmit_errors_total", static_cast<double>(tx_errs), {{"interface", interface}});
                MetricRegistry::instance().update_metric("sysmon_network_transmit_drop_total", static_cast<double>(tx_drop), {{"interface", interface}});
            }
        }
    }
}

} // namespace sysmon
