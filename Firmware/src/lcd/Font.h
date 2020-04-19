#pragma once

#include "etl/string_view.h"
#include "hardware/lcd/SpiInterface.h"
#include <cstdint>
#include <etl/array.h>
#include <etl/array_view.h>
#include <sys/_stdint.h>

namespace lcd
{

class Font
{
public:
    using GlyphView = etl::array_view<const uint8_t>;
    using MapView = etl::array_view<const uint16_t>;

    Font( uint8_t numberOfCharacters, uint8_t firstCharacterOffset, GlyphView data, MapView map,
        uint8_t height, uint8_t letterSpacingWidth );

    Font& operator=( const Font& other );

    uint16_t getHeight() const;
    uint8_t getLetterSpacingWidth() const;

    uint16_t getCharWidth( char c ) const;
    uint16_t getStringWidth( const etl::string_view& string ) const;

    GlyphView getGlyph( char c ) const;

private:
    bool isCharAvailable( char c ) const;

    const uint8_t numberOfCharacters_;
    const uint8_t firstCharacterOffset_;

    const GlyphView data_;
    const MapView map_;
    const uint16_t height_;
    const uint8_t bytesPerColumn_;
    const uint8_t letterSpacingWidth_;
};

namespace font
{
extern Font nokia_8p;
}

}  // namespace lcd
