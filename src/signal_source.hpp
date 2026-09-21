#pragma once

struct VehicleSnapshot {
    double speed_kmh;
    double battery_temp_c;
    double soc_percent;
};

class ISignalSource {
public:
    virtual ~ISignalSource() = default;
    virtual VehicleSnapshot read() = 0;
};