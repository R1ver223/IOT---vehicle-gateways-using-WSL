#include "can_signal.hpp"

#include <gtest/gtest.h>

TEST(CanSignal, EncodeDecodeRoundTrip) {
    CanSignal speed{0, 16, 0.01, 0.0};
    CanFrame frame{};
    speed.encode(frame, 92.34);
    EXPECT_NEAR(speed.decode(frame), 92.34, 0.01);
}

TEST(CanSignal, RespectsBitOffset) {
    // Two 8-bit signals packed into the same frame, like battery temp + SoC.
    CanSignal temp{0, 8, 1.0, -40.0};
    CanSignal soc{8, 8, 0.5, 0.0};

    CanFrame frame{};
    temp.encode(frame, 37.0);
    soc.encode(frame, 80.0);

    EXPECT_NEAR(temp.decode(frame), 37.0, 1.0);
    EXPECT_NEAR(soc.decode(frame), 80.0, 0.5);
}

TEST(CanSignal, ClampsOutOfRangeValues) {
    CanSignal soc{8, 8, 0.5, 0.0};  // max representable is 127.5
    CanFrame frame{};
    soc.encode(frame, 999.0);
    EXPECT_LE(soc.decode(frame), 127.5);
}