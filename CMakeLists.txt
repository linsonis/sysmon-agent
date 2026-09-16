cmake_minimum_required(VERSION 3.14)
project(sysmon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(SYSMON_BUILD_TESTS "Build unit tests" ON)

# Find ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Fetch cpp-httplib for HTTP metrics exporter
include(FetchContent)
FetchContent_Declare(
    cpp-httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.tar.gz
)
FetchContent_MakeAvailable(cpp-httplib)

# Source files skeleton
set(SOURCES
    src/main.cpp
    src/common/config.cpp
    src/common/logging.cpp
    src/metrics/metric.cpp
    src/metrics/registry.cpp
    src/metrics/prometheus.cpp
    src/scheduler/scheduler.cpp
    src/http/server.cpp
    src/collectors/cpu.cpp
    src/collectors/memory.cpp
    src/collectors/process.cpp
    src/collectors/network.cpp
    src/collectors/disk.cpp
    src/collectors/filesystem.cpp
    src/collectors/systemd.cpp
    src/collectors/containers.cpp
)

add_executable(sysmon ${SOURCES})

target_link_libraries(sysmon PRIVATE ${CURSES_LIBRARIES} httplib::httplib pthread)

target_include_directories(sysmon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(SYSMON_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(sysmon_tests
        tests/cpu_tests.cpp
        tests/memory_tests.cpp
        tests/parser_tests.cpp
        tests/metric_tests.cpp
        src/metrics/metric.cpp
        src/metrics/registry.cpp
        src/metrics/prometheus.cpp
        src/common/config.cpp
        src/common/logging.cpp
        src/collectors/cpu.cpp
        src/collectors/memory.cpp
    )

    target_link_libraries(sysmon_tests PRIVATE GTest::gtest_main pthread)
    target_include_directories(sysmon_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    include(GoogleTest)
    gtest_discover_tests(sysmon_tests)
endif()
