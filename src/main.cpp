#include <mqtt/client.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>

int main() {
    const std::string server = "tcp://localhost:1883";
    const std::string vin    = "WBA00000000000001";  // fake VIN
    const std::string base   = "fleet/" + vin;

    mqtt::client client(server, "gateway-" + vin);

    // Last Will: the broker publishes this if we vanish without disconnecting
    auto will = mqtt::message(base + "/status", "offline", 1, true);
    auto opts = mqtt::connect_options_builder()
                    .clean_session(true)
                    .keep_alive_interval(std::chrono::seconds(10))
                    .will(mqtt::will_options(will))
                    .finalize();

    try {
        client.connect(opts);
        client.publish(mqtt::make_message(base + "/status", "online", 1, true));
        std::cout << "Connected, publishing to " << base << "\n";

        for (int t = 0;; ++t) {
            double speed = 60.0 + 40.0 * std::sin(t / 5.0);  // simulated km/h
            std::string payload = "{\"speed_kmh\":" + std::to_string(speed) + "}";
            client.publish(mqtt::make_message(base + "/telemetry/speed", payload, 0, false));
            std::cout << "sent " << payload << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT error: " << e.what() << "\n";
        return 1;
    }
}