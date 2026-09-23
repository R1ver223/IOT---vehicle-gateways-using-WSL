#pragma once
#include <memory>
#include <utility>

#include "can_bus.hpp"
#include "signal_source.hpp"
#include "vehicle_signals.hpp"

// Turns raw CAN frames into a VehicleSnapshot.
class CanSignalSource : public ISignalSource {
public:
    explicit CanSignalSource(std::unique_ptr<ICanBus> bus) : bus_(std::move(bus)) {}

    VehicleSnapshot read() override {
        for (const CanFrame& f : bus_->poll()) {
            switch (f.id) {
                case can_db::kSpeedId:
                    snapshot_.speed_kmh = can_db::kSpeed.decode(f);
                    break;
                case can_db::kBatteryId:
                    snapshot_.battery_temp_c = can_db::kBatteryTemp.decode(f);
                    snapshot_.soc_percent = can_db::kSoc.decode(f);
                    break;
                default:
                    break;  // unknown message, ignore
            }
        }
        return snapshot_;
    }

private:
    std::unique_ptr<ICanBus> bus_;
    VehicleSnapshot snapshot_{};
};