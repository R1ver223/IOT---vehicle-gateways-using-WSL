#pragma once
#include <mqtt/async_client.h>

#include <functional>
#include <string>

#include "signal_source.hpp"

// Called when a command arrives: (command_name, payload) -> result string
using CommandHandler = std::function<std::string(const std::string&, const std::string&)>;

class MqttPublisher : public virtual mqtt::callback {
public:
    MqttPublisher(const std::string& server, std::string vin, std::string certs_dir);

    void connect();
    void disconnect(); 
    void publish(const VehicleSnapshot& snapshot);
    void set_command_handler(CommandHandler handler);

    // mqtt callback overrides - fired by the Paho library on its own thread
    void message_arrived(mqtt::const_message_ptr msg) override;
    void connection_lost(const std::string& cause) override;

private:
    void publish_value(const std::string& name, double value);

    std::string vin_;
    std::string base_;
    std::string certs_dir_;
    mqtt::async_client client_;
    CommandHandler command_handler_;
};