#pragma once

#include <string>

namespace sysmon {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& instance();
    void set_level(LogLevel level);
    void log(LogLevel level, const std::string& message);

    template<typename... Args>
    void debug(const std::string& fmt, Args&&... args) {
        log(LogLevel::DEBUG, fmt);
    }

    template<typename... Args>
    void info(const std::string& fmt, Args&&... args) {
        log(LogLevel::INFO, fmt);
    }

    template<typename... Args>
    void warn(const std::string& fmt, Args&&... args) {
        log(LogLevel::WARN, fmt);
    }

    template<typename... Args>
    void error(const std::string& fmt, Args&&... args) {
        log(LogLevel::ERROR, fmt);
    }

private:
    LogLevel current_level_ = LogLevel::INFO;
};

} // namespace sysmon
