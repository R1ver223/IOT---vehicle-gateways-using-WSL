#pragma once
#include <cmath>
#include <vector>

#include "can_bus.hpp"
#include "vehicle_signals.hpp"

// Plays the role of the ECUs: encodes physical values into raw CAN frames.
class SimulatedCanBus : public ICanBus {
public:
    std::vector<CanFrame> poll() override {
        const double t = tick_++;

        CanFrame speed;
        speed.id = can_db::kSpeedId;
        speed.dlc = 2;
        can_db::kSpeed.encode(speed, 60.0 + 40.0 * std::sin(t / 5.0));

        CanFrame battery;
        battery.id = can_db::kBatteryId;
        battery.dlc = 2;
        can_db::kBatteryTemp.encode(battery, 35.0 + 5.0 * std::sin(t / 20.0));
        can_db::kSoc.encode(battery, std::max(0.0, 80.0 - t * 0.01));

        return {speed, battery};
    }

private:
    int tick_ = 0;
};