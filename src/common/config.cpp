#include "config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace sysmon {

static inline std::string trim(const std::string& s) {
    auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); });
    auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
    return (start < end ? std::string(start, end) : std::string());
}

bool Config::load_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << path << ", using defaults.\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        std::size_t colon_pos = trimmed.find(':');
        if (colon_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(trimmed.substr(0, colon_pos));
        std::string val = trim(trimmed.substr(colon_pos + 1));

        // Strip inline quotes if any
        if (val.size() >= 2 && ((val.front() == '"' && val.back() == '"') || (val.front() == '\'' && val.back() == '\''))) {
            val = val.substr(1, val.size() - 2);
        }

        if (key == "http_port") {
            try {
                http_port = std::stoi(val);
            } catch (...) {}
        } else if (key == "collection_interval_ms") {
            try {
                collection_interval_ms = std::stoi(val);
            } catch (...) {}
        } else if (key == "enable_docker") {
            enable_docker = (val == "true" || val == "yes" || val == "1");
        } else if (key == "log_level") {
            log_level = val;
        }
    }

    return true;
}

} // namespace sysmon
