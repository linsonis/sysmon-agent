#pragma once

#include <functional>
#include <atomic>
#include <thread>
#include <chrono>

namespace sysmon {

class Scheduler {
public:
    void start(std::chrono::milliseconds interval, std::function<void()> task);
    void stop();

private:
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace sysmon
