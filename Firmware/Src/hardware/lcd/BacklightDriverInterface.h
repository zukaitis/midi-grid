#pragma once

#include <cstdint>

namespace hardware
{

namespace lcd
{

class BacklightDriverInterface
{
public:
    virtual ~BacklightDriverInterface() = default;

    virtual void initialize() = 0;
    virtual void setIntensity( const uint8_t intensity ) = 0;

    virtual uint8_t getMaximumIntensity() const = 0;
};

}

}