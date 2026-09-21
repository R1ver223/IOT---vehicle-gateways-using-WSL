#include "mqtt_publisher.hpp"

#include <chrono>
#include <cstdio>
#include <utility>

MqttPublisher::MqttPublisher(const std::string& server, std::string vin)
    : vin_(std::move(vin)),
      base_("fleet/" + vin_),
      client_(server, "gateway-" + vin_) {}

void MqttPublisher::connect() {
    auto will = mqtt::message(base_ + "/status", "offline", 1, true);
    auto opts = mqtt::connect_options_builder()
                    .clean_session(true)
                    .keep_alive_interval(std::chrono::seconds(10))
                    .will(mqtt::will_options(will))
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