#include "scheduler.hpp"

namespace sysmon {

void Scheduler::start(std::chrono::milliseconds interval, std::function<void()> task) {
    running_ = true;
    worker_ = std::thread([this, interval, task]() {
        while (running_) {
            task();
            std::this_thread::sleep_for(interval);
        }
    });
}

void Scheduler::stop() {
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
}

} // namespace sysmon
