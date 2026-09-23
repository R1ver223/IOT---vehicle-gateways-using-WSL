#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

#include "can_frame.hpp"

// Describes one signal inside a CAN frame (Intel / little-endian layout,
// like a DBC signal declared with "@1+").
// physical = raw * factor + offset
struct CanSignal {
    unsigned start_bit;  // position of the least significant bit
    unsigned length;     // number of bits (1..32 supported)
    double factor;
    double offset;

    uint64_t mask() const { return (1ULL << length) - 1; }

    static uint64_t load(const CanFrame& f) {
        uint64_t word = 0;
        for (int i = 7; i >= 0; --i) word = (word << 8) | f.data[i];
        return word;
    }

    static void store(CanFrame& f, uint64_t word) {
        for (int i = 0; i < 8; ++i) f.data[i] = static_cast<uint8_t>(word >> (8 * i));
    }

    double decode(const CanFrame& f) const {
        const uint64_t raw = (load(f) >> start_bit) & mask();
        return static_cast<double>(raw) * factor + offset;
    }

    void encode(CanFrame& f, double physical) const {
        const double scaled = std::round((physical - offset) / factor);
        const auto raw = static_cast<uint64_t>(std::clamp(scaled, 0.0, static_cast<double>(mask())));
        uint64_t word = load(f);
        word &= ~(mask() << start_bit);
        word |= raw << start_bit;
        store(f, word);
    }
};