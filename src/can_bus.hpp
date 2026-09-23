#pragma once
#include <vector>

#include "can_frame.hpp"

class ICanBus {
public:
    virtual ~ICanBus() = default;
    // Returns all frames received since the last call.
    virtual std::vector<CanFrame> poll() = 0;
};