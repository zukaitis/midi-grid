#pragma once

#include <cstdint>
#include <etl/array_view.h>

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
    virtual void writeCommand( uint8_t command ) const = 0;
    virtual void writeData( const uint8_t& data, const uint32_t size ) const = 0;
    virtual void writeData( const etl::array_view<uint8_t>& data ) const = 0;
};

}

}

