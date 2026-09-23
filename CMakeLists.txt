cmake_minimum_required(VERSION 3.20)
project(vehicle_gateway LANGUAGES CXX)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PahoMqttCpp REQUIRED)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(tests tests/can_signal_test.cpp)
target_link_libraries(tests PRIVATE GTest::gtest_main)
target_include_directories(tests PRIVATE src)
include(GoogleTest)
gtest_discover_tests(tests)

add_executable(gateway src/main.cpp src/mqtt_publisher.cpp)
target_compile_options(gateway PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(gateway PRIVATE PahoMqttCpp::paho-mqttpp3)