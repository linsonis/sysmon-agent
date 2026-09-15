#include "containers.hpp"
#include "../metrics/registry.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace sysmon {

static std::string query_docker_socket(const std::string& path) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        return "";
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, "/var/run/docker.sock", sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return "";
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
    if (body_pos != std::string::npos) {
        return response.substr(body_pos + 4);
    }
    return "";
}

void ContainerCollector::collect() {
    std::string json_response = query_docker_socket("/containers/json?all=true");
    if (json_response.empty()) {
        // Docker daemon not running or socket inaccessible
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
