#pragma once
#include <algorithm>
#include <cmath>

#include "signal_source.hpp"

class SimulatedSource : public ISignalSource {
public:
    VehicleSnapshot read() override {
        const double t = tick_++;
        return {
            60.0 + 40.0 * std::sin(t / 5.0),
            35.0 + 5.0 * std::sin(t / 20.0),
            std::max(0.0, 80.0 - t * 0.01),
        };
    }

private:
    int tick_ = 0;
};