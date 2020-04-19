#include "lcd/Font.h"

namespace lcd
{

static const etl::array<uint8_t, 0> emptyArray = {};

Font::Font( uint8_t numberOfCharacters, uint8_t firstCharacterOffset, GlyphView data, MapView map,
        uint8_t height, uint8_t letterSpacingWidth ):
    numberOfCharacters_( numberOfCharacters ),
    firstCharacterOffset_( firstCharacterOffset ),
    data_( data ),
    map_( map ),
    height_( height ),
    bytesPerColumn_( (height_ + 7) / 8 ), // makeshift ceil() function
    letterSpacingWidth_( letterSpacingWidth )
{
}

Font& Font::operator=( const Font& /*other*/ )
{
    return *this;
}

uint16_t Font::getHeight() const
{
    return height_;
}

uint8_t Font::getLetterSpacingWidth() const
{
    return letterSpacingWidth_;
}

uint16_t Font::getCharWidth( const char c ) const
{
    uint16_t width = 0;

    if (isCharAvailable( c ))
    {
        const uint8_t index = c - firstCharacterOffset_;
        width = (map_.at( index + 1 ) - map_.at( index )) / bytesPerColumn_;
    }

    return width;
}

uint16_t Font::getStringWidth( const etl::string_view& string ) const
{
    uint16_t width = 0;
    for (char c : string)
    {
        width += getCharWidth( c );
    }

    width += (string.length() - 1) * letterSpacingWidth_;
    return width;
}

Font::GlyphView Font::getGlyph( char c ) const
{
    GlyphView array( emptyArray );

    if (isCharAvailable( c ))
    {
        const uint8_t index = c - firstCharacterOffset_;
        array = GlyphView( &data_.at( map_.at( index ) ), getCharWidth( c ) * bytesPerColumn_ );
    }

    return array;
}

bool Font::isCharAvailable( const char c ) const
{
    return ((c >= firstCharacterOffset_) && (c < (firstCharacterOffset_ + numberOfCharacters_)));
}

}  // namespace lcd
