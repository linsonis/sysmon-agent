#include "server.hpp"
#include "../metrics/prometheus.hpp"
#include <httplib.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

namespace sysmon {

struct HttpServerImpl {
    httplib::Server svr;
    std::thread server_thread;
};

HttpServer::HttpServer() = default;

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port) {
    if (impl_) {
        return false;  // already started
    }
    auto impl = std::make_unique<HttpServerImpl>();

    impl->svr.Get("/metrics", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(PrometheusExporter::render(), "text/plain; version=0.0.4");
    });

    impl->svr.Get("/healthz", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    // httplib's default options include SO_REUSEPORT, which lets a second
    // instance bind the same port silently. Keep only SO_REUSEADDR.
    impl->svr.set_socket_options([](socket_t sock) {
        int yes = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(yes));
    });

    // Bind synchronously so a taken port is reported to the caller and stop()
    // cannot race a listener that has not started yet.
    if (!impl->svr.bind_to_port("0.0.0.0", port)) {
        std::cerr << "HTTP server failed to bind port " << port << "\n";
        return false;
    }

    HttpServerImpl* raw = impl.get();
    impl->server_thread = std::thread([raw]() {
        raw->svr.listen_after_bind();
    });
    impl_ = std::move(impl);

    for (int i = 0; i < 200 && !impl_->svr.is_running(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true;
}

void HttpServer::stop() {
    if (!impl_) {
        return;
    }
    impl_->svr.stop();
    if (impl_->server_thread.joinable()) {
        impl_->server_thread.join();
    }
    impl_.reset();
}

} // namespace sysmon
