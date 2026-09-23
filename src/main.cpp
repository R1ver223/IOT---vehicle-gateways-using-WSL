#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "can_signal_source.hpp"
#include "mqtt_publisher.hpp"
#include "simulated_can_bus.hpp"
int main() {
   std::unique_ptr<ISignalSource> source = std::make_unique<CanSignalSource>(std::make_unique<SimulatedCanBus>());
MqttPublisher publisher("ssl://localhost:8883", "WBA00000000000001",
                         "/home/r1ver223/projects/vehicle-gateway/certs");

    try {
        publisher.connect();
        std::cout << "Gateway online\n";
        for (;;) {
            publisher.publish(source->read());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT error: " << e.what() << "\n";
        return 1;
    }
}