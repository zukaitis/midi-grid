#pragma once

#include <cstdint>

namespace lcd
{

class BacklightInterface
{
public:
    virtual ~BacklightInterface() = default;

    virtual void initialize() = 0;
    virtual void setIntensity( const uint8_t intensity ) = 0;

    virtual uint8_t getMaximumIntensity() const = 0;
};

}
