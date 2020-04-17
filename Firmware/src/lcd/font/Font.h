#pragma once

#include <cstdint>
#include <etl/array_view.h>

namespace lcd
{

class Font
{
    Font( uint8_t numberOfCharacters, uint8_t firstCharacterOffset, etl::array_view<uint8_t> data, etl::array_view<uint16_t> map,
        uint8_t bytesPerColumn, uint8_t  );

    using DataView = etl::array_view<uint8_t>;
    using MapView = etl::array_view<uint16_t>;

private:
    uint8_t numberOfCharacters_;
    uint8_t firstCharacterOffset_;

    DataView data_;
    MapView map_;
    uint8_t bytesPerColumn_;
    uint8_t spaceWidth_;
};

}
