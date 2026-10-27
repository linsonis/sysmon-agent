#pragma once

#include <memory>
#include <string>

namespace sysmon {

struct HttpServerImpl;

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    bool start(int port);
    void stop();

private:
    std::unique_ptr<HttpServerImpl> impl_;
};

} // namespace sysmon
