#include "mqtt_publisher.hpp"

#include <chrono>
#include <cstdio>
#include <utility>

MqttPublisher::MqttPublisher(const std::string& server, std::string vin, std::string certs_dir)
    : vin_(std::move(vin)),
      base_("fleet/" + vin_),
      certs_dir_(std::move(certs_dir)),
      client_(server, "gateway-" + vin_) {}

void MqttPublisher::connect() {
    auto will = mqtt::message(base_ + "/status", "offline", 1, true);

    auto ssl_opts = mqtt::ssl_options_builder()
                         .trust_store(certs_dir_ + "/ca.crt")
                         .key_store(certs_dir_ + "/client.crt")
                         .private_key(certs_dir_ + "/client.key")
                         .error_handler([](const std::string& msg) {
                             fprintf(stderr, "TLS error: %s\n", msg.c_str());
                         })
                         .finalize();

    auto opts = mqtt::connect_options_builder()
                    .clean_session(true)
                    .keep_alive_interval(std::chrono::seconds(10))
                    .will(mqtt::will_options(will))
                    .ssl(ssl_opts)
                    .finalize();

    client_.connect(opts);
    client_.publish(mqtt::make_message(base_ + "/status", "online", 1, true));
}

void MqttPublisher::publish(const VehicleSnapshot& s) {
    publish_value("speed_kmh", s.speed_kmh);
    publish_value("battery_temp_c", s.battery_temp_c);
    publish_value("soc_percent", s.soc_percent);
}

void MqttPublisher::publish_value(const std::string& name, double value) {
    char payload[64];
    std::snprintf(payload, sizeof payload, "{\"%s\":%.1f}", name.c_str(), value);
    client_.publish(mqtt::make_message(base_ + "/telemetry/" + name, payload, 0, false));
}