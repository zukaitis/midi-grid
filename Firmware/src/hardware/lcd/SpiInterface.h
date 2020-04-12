#pragma once

#include <cstdint>
#include <etl/array.h>
#include <etl/array_view.h>

namespace lcd // TODO(anyone): quick hack, remove it once possible
{
using Pixel = etl::array<uint8_t, 3>;
using PixelView = etl::array_view<const Pixel>;
using RawDataView = etl::array_view<const uint8_t>;
}

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
    virtual void writeData( const etl::array_view<const uint8_t>& data ) const = 0;
    virtual void writeData( const ::lcd::PixelView& data) const = 0;
};

}

}

