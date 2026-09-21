cmake_minimum_required(VERSION 3.20)
project(vehicle_gateway LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PahoMqttCpp REQUIRED)

add_executable(gateway src/main.cpp src/mqtt_publisher.cpp)
target_compile_options(gateway PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(gateway PRIVATE PahoMqttCpp::paho-mqttpp3)