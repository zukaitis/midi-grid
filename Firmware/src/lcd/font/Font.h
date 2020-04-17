#pragma once

#include <cstdint>
#include <etl/array.h>
#include <etl/array_view.h>

namespace lcd
{

using DataView = etl::array_view<uint8_t>;
using MapView = etl::array_view<uint16_t>;

class Font
{
public:
    Font( uint8_t numberOfCharacters, uint8_t firstCharacterOffset, DataView data, MapView map,
        uint8_t bytesPerColumn, uint8_t gapWidth );

private:
    uint8_t numberOfCharacters_;
    uint8_t firstCharacterOffset_;

    DataView data_;
    MapView map_;
    uint8_t bytesPerColumn_;
    uint8_t gapWidth_;
};

}  // namespace lcd
