#pragma once

#include "hardware/lcd/BacklightDriverInterface.h"

#include <etl/array.h>

namespace hardware
{

namespace lcd
{

class BacklightDriver: public BacklightDriverInterface
{
public:
    BacklightDriver();
    virtual ~BacklightDriver();

    void initialize() override;
    void setIntensity( const uint8_t intensity ) override;
    uint8_t getMaximumIntensity() const override;

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    static etl::array<uint32_t, 32> outputBuffer_;

    static constexpr uint8_t maximumIntensity_ = 64;
};

}

}