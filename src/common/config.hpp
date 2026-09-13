#pragma once

#include <string>

namespace sysmon {

struct Config {
    int http_port = 9100;
    int collection_interval_ms = 1000;
    bool enable_docker = true;
    std::string log_level = "info";

    bool load_from_file(const std::string& path);
};

} // namespace sysmon
