#include "logging.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace sysmon {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::set_level(LogLevel level) {
    current_level_ = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < current_level_) {
        return;
    }

    std::string level_str = "INFO";
    switch (level) {
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO:  level_str = "INFO";  break;
        case LogLevel::WARN:  level_str = "WARN";  break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
    }

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
              << "[" << level_str << "] " << message << "\n";
}

} // namespace sysmon
