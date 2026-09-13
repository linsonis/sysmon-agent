#include <gtest/GTest.h>
#include "common/config.hpp"

TEST(ParserTest, DefaultConfig) {
    sysmon::Config config;
    EXPECT_EQ(config.http_port, 9100);
}
