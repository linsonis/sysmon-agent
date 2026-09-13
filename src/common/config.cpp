#include "config.hpp"
#include <iostream>
#include <fstream>

namespace sysmon {

bool Config::load_from_file(const std::string& path) {
    // Skeleton: parse configuration file or use defaults
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << path << ", using defaults.\n";
        return false;
    }
    // TODO: parse YAML/key-value pairs in full implementation
    return true;
}

} // namespace sysmon
