#include "containers.hpp"
#include "../metrics/registry.hpp"
#include "../common/logging.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <cerrno>

namespace sysmon {

// Returns false and fills `error` if the Docker socket cannot be queried.
static bool query_docker_socket(const std::string& path, std::string& body, std::string& error) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        error = std::string("socket(): ") + std::strerror(errno);
        return false;
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, "/var/run/docker.sock", sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        error = std::string("cannot connect to /var/run/docker.sock: ") + std::strerror(errno);
        close(sock);
        return false;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: localhost\r\nAccept: */*\r\nConnection: close\r\n\r\n";
    send(sock, request.c_str(), request.length(), 0);

    std::string response;
    char buffer[4096];
    ssize_t bytes_read = 0;
    while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        response += buffer;
    }

    close(sock);

    // Find double newline separating headers and body
    std::size_t body_pos = response.find("\r\n\r\n");
    if (body_pos == std::string::npos) {
        error = "malformed response from Docker daemon";
        return false;
    }
    body = response.substr(body_pos + 4);
    return true;
}

void ContainerCollector::collect() {
    // -1 = not yet checked, 0 = unavailable, 1 = available. Atomic because the
    // scheduler thread and the TUI loop both call collect().
    static std::atomic<int> last_state{-1};

    std::string json_response;
    std::string error;
    bool ok = query_docker_socket("/containers/json?all=true", json_response, error);

    int state = ok ? 1 : 0;
    if (last_state.exchange(state) != state) {
        if (ok) {
            Logger::instance().info("Docker daemon is reachable, collecting container metrics");
        } else {
            Logger::instance().warn("Docker metrics unavailable: " + error);
        }
    }
    MetricRegistry::instance().update_metric("sysmon_docker_up", ok ? 1.0 : 0.0);
    if (!ok) {
        return;
    }

    double total_containers = 0;
    double running_containers = 0;
    double stopped_containers = 0;
    double paused_containers = 0;

    // Simple JSON array object scanner
    std::size_t pos = 0;
    while ((pos = json_response.find("\"State\":\"", pos)) != std::string::npos) {
        pos += 9;
        std::size_t end_pos = json_response.find("\"", pos);
        if (end_pos != std::string::npos) {
            std::string state = json_response.substr(pos, end_pos - pos);
            total_containers++;
            if (state == "running") {
                running_containers++;
            } else if (state == "paused") {
                paused_containers++;
            } else {
                stopped_containers++;
            }
        }
    }

    MetricRegistry::instance().update_metric("sysmon_docker_containers_total", total_containers);
    MetricRegistry::instance().update_metric("sysmon_docker_containers_running", running_containers);
    MetricRegistry::instance().update_metric("sysmon_docker_containers_stopped", stopped_containers);
    MetricRegistry::instance().update_metric("sysmon_docker_containers_paused", paused_containers);
}

} // namespace sysmon
