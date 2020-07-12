#pragma once

#include <cstdint>

namespace lcd
{

class BacklightInterface
{
public:
    virtual ~BacklightInterface() = default;

    virtual void initialize() {};
    virtual void setIntensity( uint8_t intensity ) {};

    virtual uint8_t maxIntensity() { return maxIntensity_; };

private:
    static const uint8_t maxIntensity_ = 64;
};

} // namespace lcd
