#pragma once

#include <string>

namespace sysmon {

class HttpServer {
public:
    bool start(int port);
    void stop();

private:
    // impl pointer or httplib server holder
};

} // namespace sysmon
