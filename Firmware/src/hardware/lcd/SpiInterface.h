#pragma once

#include <cstdint>

namespace hardware
{

namespace lcd
{

class SpiInterface
{
public:
    virtual ~SpiInterface() = default;

    virtual void initialize() const = 0;

    virtual void reset() const = 0;
    virtual void writeCommand( const uint8_t& command, const uint32_t size = 1 ) const = 0;
    virtual void writeData( const uint8_t& data, const uint32_t size ) const = 0;
};

}

}

