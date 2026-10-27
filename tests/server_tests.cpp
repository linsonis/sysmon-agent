#include <gtest/gtest.h>
#include <httplib.h>
#include "http/server.hpp"

TEST(HttpServerTest, StopReleasesPort) {
    const int port = 19111;

    sysmon::HttpServer first;
    ASSERT_TRUE(first.start(port));
    {
        httplib::Client client("127.0.0.1", port);
        auto res = client.Get("/healthz");
        ASSERT_TRUE(res);
        EXPECT_EQ(res->status, 200);
    }
    first.stop();

    // Port must be free again once stop() returns.
    sysmon::HttpServer second;
    EXPECT_TRUE(second.start(port));
}

TEST(HttpServerTest, StartFailsWhenPortTaken) {
    const int port = 19112;

    sysmon::HttpServer first;
    ASSERT_TRUE(first.start(port));

    sysmon::HttpServer second;
    EXPECT_FALSE(second.start(port));
}
