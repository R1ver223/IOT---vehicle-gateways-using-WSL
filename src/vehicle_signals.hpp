#pragma once
#include <cstdint>

#include "can_signal.hpp"

namespace can_db {

// Message 0x1A0 "VehicleSpeed"
inline constexpr uint32_t kSpeedId = 0x1A0;
inline constexpr CanSignal kSpeed{0, 16, 0.01, 0.0};   // 0.01 km/h per bit

// Message 0x2B0 "BatteryStatus"
inline constexpr uint32_t kBatteryId = 0x2B0;
inline constexpr CanSignal kBatteryTemp{0, 8, 1.0, -40.0};  // -40..215 C
inline constexpr CanSignal kSoc{8, 8, 0.5, 0.0};            // 0..127.5 %

}  // namespace can_db