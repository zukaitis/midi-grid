#pragma once

#include <cstdint>
#include <etl/array.h>
#include <etl/array_view.h>

namespace lcd
{

class Image
{
public:
    using DataView = etl::array_view<const uint8_t>;

    Image( DataView data, uint16_t width, uint16_t height ):
        data_( data ),
        width_( width ),
        height_( height )
    {
    }

    Image():
        data_(),
        width_( 0 ),
        height_( 0 )
    {
    }

    uint16_t getWidth() const
    {
        return width_;
    }

    uint16_t getHeight() const
    {
        return height_;
    }

    DataView getData() const
    {
        return data_;
    }

private:
    DataView data_;
    uint16_t width_;
    uint16_t height_;
};

}  // namespace lcd
