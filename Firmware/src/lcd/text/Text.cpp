#include "lcd/text/Text.h"

#include "lcd/text/Font.h"
#include "lcd/Parameters.h"
#include "lcd/image/ImageInterface.h"

namespace lcd
{

Text::Text( ImageInterface* image ):
    image_( *image )
{
}

Text::~Text() = default;

uint16_t Text::print( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    const bool createImage = (false == image_.isBeingDrawn());

    if (createImage)
    {
        image_.createNew( format.font().getStringWidth( string ), format.font().getHeight() );
    }

    const Coordinates adjustedCoords = calculateCoordinates( string, coords, format );
    uint16_t width = 0;

    if (createImage)
    {
        width = putString( string, {0, 0}, format );
        image_.display( adjustedCoords );
        image_.endDrawing();
    }
    else
    {
        width = putString( string, adjustedCoords, format );
    }

    return width;
}

uint16_t Text::print( const etl::string_view& string, const uint16_t y, const Format& format )
{
    return print( string, {calculateX( format.justification() ), y}, format );
}

uint16_t Text::putString( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    Coordinates charCoords = coords;
    const Font& font = format.font();
    const uint8_t textColorIndex = image_.assignColorIndex( format.textColor() );
    uint8_t backgroundColorIndex = 0;
    if (format.isBackgroundColorSet())
    {
        backgroundColorIndex = image_.assignColorIndex( format.backgroundColor() );
    }

    charCoords.x += putChar( string.at(0), charCoords, font, textColorIndex, backgroundColorIndex );
    for (uint32_t c = 1; c < string.length(); c++)
    {
        charCoords.x += putCharacterSpace( charCoords, font, backgroundColorIndex );
        charCoords.x += putChar( string.at(c), charCoords, font, textColorIndex, backgroundColorIndex );
    }

    return charCoords.x - coords.x;
}

uint16_t Text::putChar( const char c, const Coordinates& coords, const lcd::Font& font,
    const uint8_t textColorIndex, const uint8_t backgroundColorIndex )
{
    const Font::GlyphView data = font.getGlyph( c );
    uint16_t height = font.getHeight();
    const uint16_t bytesPerColumn = (height + 7) / 8;
    if ((coords.y + height) > image_.height())
    {
        height = image_.height() - coords.y;
    }

    uint16_t width = data.size() / bytesPerColumn;
    if ((coords.x + width) > image_.width())
    {
        width = image_.width() - coords.x;
    }

    for (uint16_t x = 0; x < width; x++)
    {
        for (uint16_t y = 0; y < height; y++)
        {
            const uint8_t byte = data.at( x * bytesPerColumn + y / 8U );
            const bool pixelActive = ((static_cast<uint8_t>(byte >> (y % 8U)) & 0x01U) != 0U);
            
            if (pixelActive)
            {
                image_.putPixel( {static_cast<uint16_t>(coords.x + x), static_cast<uint16_t>(coords.y + y)}, textColorIndex );
            }
            else if (0 != backgroundColorIndex)
            {
                image_.putPixel( {static_cast<uint16_t>(coords.x + x), static_cast<uint16_t>(coords.y + y)}, backgroundColorIndex );
            }
        }
    }

    return width;
}

uint16_t Text::putCharacterSpace( const Coordinates& coords, const lcd::Font& font, const uint8_t backgroundColorIndex )
{
    uint16_t height = font.getHeight();
    if ((coords.y + height) > image_.height())
    {
        height = image_.height() - coords.y;
    }

    uint16_t width = font.getLetterSpacingWidth();
    if ((coords.x + width) > image_.width())
    {
        width = image_.width() - coords.x;
    }

    if (0 != backgroundColorIndex)
    {
        for (uint16_t x = 0; x < width; x++)
        {
            for (uint16_t y = 0; y < height; y++)
            {
                image_.putPixel( {static_cast<uint16_t>(coords.x + x), static_cast<uint16_t>(coords.y + y)}, backgroundColorIndex );
            }
        }
    }

    return width;
}

uint16_t Text::calculateX( const Justification justification )
{
    const uint16_t imageWidth = image_.isBeingDrawn() ? image_.width() : lcd::parameters::width;

    uint16_t x = 0;
    switch (justification)
    {
        case Justification::RIGHT:
            x = imageWidth - 1;
            break;
        case Justification::CENTER:
            x = imageWidth / 2;
            break;
        case Justification::LEFT:
        default:
            break;
    }

    return x;
}

Coordinates Text::calculateCoordinates( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    Coordinates calculated = coords;

    switch (format.justification())
    {
        case Justification::RIGHT:
            {
                const uint16_t textwidth = format.font().getStringWidth( string );
                const uint16_t x = ((coords.x + 1U) >= textwidth) ? (coords.x + 1U - textwidth) : 0;
                calculated = {x, coords.y};
            }
            break;
        case Justification::CENTER:
            {
                const uint16_t distanceFromMiddle = format.font().getStringWidth( string ) / 2;
                const uint16_t x = (coords.x >= distanceFromMiddle) ? (coords.x - distanceFromMiddle) : 0;
                calculated = {x, coords.y};
            }
            break;
        case Justification::LEFT:
        default:
            break;
    }

    return calculated;
}

}  // namespace lcd
