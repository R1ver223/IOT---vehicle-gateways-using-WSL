#pragma once
#include <mqtt/client.h>

#include <string>

#include "signal_source.hpp"

class MqttPublisher {
public:
    MqttPublisher(const std::string& server, std::string vin);
    void connect();
    void publish(const VehicleSnapshot& snapshot);

private:
    void publish_value(const std::string& name, double value);

    std::string vin_;
    std::string base_;
    mqtt::client client_;
};