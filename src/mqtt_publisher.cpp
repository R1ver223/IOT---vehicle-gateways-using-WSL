#include "mqtt_publisher.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <utility>

MqttPublisher::MqttPublisher(const std::string& server, std::string vin, std::string certs_dir)
    : vin_(std::move(vin)),
      base_("fleet/" + vin_),
      certs_dir_(std::move(certs_dir)),
      client_(server, "gateway-" + vin_) {
    client_.set_callback(*this);
}

void MqttPublisher::set_command_handler(CommandHandler handler) {
    command_handler_ = std::move(handler);
}

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

    client_.connect(opts)->wait();
    client_.publish(mqtt::make_message(base_ + "/status", "online", 1, true))->wait();
    client_.subscribe(base_ + "/command/+", 1)->wait();
    std::cout << "Subscribed to " << base_ << "/command/+\n";
}

void MqttPublisher::disconnect() {
    client_.publish(mqtt::make_message(base_ + "/status", "offline", 1, true))->wait();
    client_.disconnect()->wait();
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
void MqttPublisher::message_arrived(mqtt::const_message_ptr msg) {
    const std::string topic = msg->get_topic();
    if (topic == base_ + "/command_result") return;  // ignore our own output

    const std::string payload = msg->to_string();

    // topic looks like: fleet/{vin}/command/lock
    const auto pos = topic.rfind('/');
    const std::string command = (pos == std::string::npos) ? topic : topic.substr(pos + 1);

    std::cout << "Command received: " << command << " (" << payload << ")\n";

    if (!command_handler_) return;
    const std::string result = command_handler_(command, payload);
    client_.publish(mqtt::make_message(base_ + "/command_result", result, 1, false));
}

void MqttPublisher::connection_lost(const std::string& cause) {
    std::cerr << "Connection lost: " << cause << "\n";
}

