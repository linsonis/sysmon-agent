#include <gtest/gtest.h>
#include "common/config.hpp"
#include <fstream>

TEST(ParserTest, DefaultConfig) {
    sysmon::Config config;
    EXPECT_EQ(config.http_port, 9100);
    EXPECT_EQ(config.collection_interval_ms, 1000);
    EXPECT_TRUE(config.enable_docker);
}

TEST(ParserTest, LoadFromFile) {
    std::string test_config_path = "test_config.yaml";
    {
        std::ofstream out(test_config_path);
        out << "http_port: 8080\n";
        out << "collection_interval_ms: 2000\n";
        out << "enable_docker: false\n";
        out << "log_level: debug\n";
    }

    sysmon::Config config;
    bool success = config.load_from_file(test_config_path);
    EXPECT_TRUE(success);
    EXPECT_EQ(config.http_port, 8080);
    EXPECT_EQ(config.collection_interval_ms, 2000);
    EXPECT_FALSE(config.enable_docker);
    EXPECT_EQ(config.log_level, "debug");

    std::remove(test_config_path.c_str());
}
