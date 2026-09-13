#include "server.hpp"
#include "../metrics/prometheus.hpp"
#include <httplib.h>
#include <iostream>
#include <thread>

namespace sysmon {

struct HttpServerImpl {
    httplib::Server svr;
    std::thread server_thread;
};

bool HttpServer::start(int port) {
    auto impl = new HttpServerImpl();
    
    impl->svr.Get("/metrics", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(PrometheusExporter::render(), "text/plain; version=0.0.4");
    });

    impl->svr.Get("/healthz", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    impl->server_thread = std::thread([impl, port]() {
        impl->svr.listen("0.0.0.0", port);
    });
    impl->server_thread.detach();

    return true;
}

void HttpServer::stop() {
    // Stop server
}

} // namespace sysmon
