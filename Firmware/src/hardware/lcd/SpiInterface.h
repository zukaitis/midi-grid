#pragma once

#include "lcd/Format.h"
#include <cstdint>
#include <etl/array_view.h>

namespace hardware
{

namespace lcd
{

using RawDataView = etl::array_view<const uint8_t>;

class SpiInterface
{
public:
    virtual ~SpiInterface() = default;

    virtual void initialize() const = 0;

    virtual void reset() const = 0;
    virtual void writeCommand( uint8_t command ) const = 0;
    virtual void writeData( const uint8_t& data, uint32_t size ) const = 0;
    virtual void writeData( const RawDataView& data ) const = 0;
    virtual void writeData( const ::lcd::PixelView& data ) const = 0;
};

}

}

