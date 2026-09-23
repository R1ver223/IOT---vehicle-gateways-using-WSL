#pragma once
#include <array>
#include <cstdint>

struct CanFrame {
    uint32_t id = 0;
    uint8_t dlc = 8;
    std::array<uint8_t, 8> data{};
};