#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>

#include "can_signal_source.hpp"
#include "mqtt_publisher.hpp"
#include "simulated_can_bus.hpp"
#include "vehicle_state.hpp"

namespace {
volatile std::sig_atomic_t g_shutdown_requested = 0;

void handle_signal(int) { g_shutdown_requested = 1; }
}  // namespace

int main(int argc, char** argv) {
    std::string vin = "WBA00000000000001";  // default
    if (argc > 1) {
        vin = argv[1];
    }

    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::unique_ptr<ISignalSource> source =
        std::make_unique<CanSignalSource>(std::make_unique<SimulatedCanBus>());

    std::string certs_dir = (std::filesystem::current_path() / "certs").string();
    MqttPublisher publisher("ssl://localhost:8883", vin, certs_dir);

    VehicleState state;
    publisher.set_command_handler([&state](const std::string& cmd, const std::string& payload) {
        return state.handle_command(cmd, payload);
    });

    try {
        publisher.connect();
        std::cout << "Gateway online for VIN " << vin << "\n";

        while (!g_shutdown_requested) {
            publisher.publish(source->read());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Shutdown requested, disconnecting cleanly...\n";
        publisher.disconnect();
        std::cout << "Disconnected.\n";
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT error: " << e.what() << "\n";
        return 1;
    }
}