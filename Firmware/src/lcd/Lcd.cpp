#include "lcd/Lcd.h"

#include "lcd/Format.h"
#include "lcd/Font.h"
#include "lcd/progressArc.h"
#include "lcd/font.h"

#include "lcd/DriverInterface.h"
#include "lcd/backlight/BacklightInterface.h"

#include <cmath>

namespace lcd
{

Lcd::Lcd( DriverInterface& driver, BacklightInterface& backlight ) :
    driver_( driver ),
    backlight_( backlight ),
    backgroundColor_( color::BLACK )
{
}

Lcd::~Lcd()
{
}

void Lcd::setBackgroundColor( const Pixel& color )
{
    backgroundColor_ = color;
}

void Lcd::clear()
{
    driver_.fill( backgroundColor_ );
}

void Lcd::clearArea( const Coordinates& corner1, const Coordinates& corner2 )
{
    driver_.fillArea( corner1, corner2, backgroundColor_ );
}

void Lcd::fillArea( const Coordinates& corner1, const Coordinates& corner2, const Pixel& color )
{
    driver_.fillArea( corner1, corner2, color );
}

void Lcd::displayImage( const uint8_t x, const uint8_t y, const ImageLegacy& image )
{
    driver_.displayImage( x, y, image );
}

void Lcd::displayImage( const Coordinates& coords, const Image& image, const Pixel& color )
{
    driver_.putImage( coords, image, {color, backgroundColor_} );
}

void Lcd::displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position )
{
    const ImageLegacy arc = { &progressArcArray.at(position)[0], 38, 32 };
    displayImage( x, y, arc );
}

void Lcd::initialize()
{
    driver_.initialize();
    backlight_.initialize();
}

void Lcd::print( const etl::string_view& string, const uint8_t x, const uint8_t y, const Justification justification )
{
    uint8_t textwidth = string.length() * FONT_WIDTH;

    switch (justification)
    {
        case Justification::RIGHT:
            if (textwidth < x)
            {
                driver_.putString( string, {static_cast<uint16_t>(x - textwidth), y} );
            }
            break;
        case Justification::CENTER:
            textwidth = textwidth / 2;
            if ((textwidth <= x) && (textwidth <= (driver_.width() - x)))
            {
                driver_.putString( string, {static_cast<uint16_t>(x - textwidth), y} );
            }
            break;
        case Justification::LEFT:
        default:
            if (textwidth < (driver_.width() - x))
            {
                driver_.putString( string, {x, y} );
            }
            break;
    }
}

void Lcd::print( const etl::string_view& string, uint8_t y, const Justification justification )
{
    print( string, calculateX( justification ), y, justification );
}

void Lcd::print( const etl::string_view& string, const uint8_t x, const uint8_t y )
{
    print( string, x, y, Justification::LEFT );
}

void Lcd::print( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    Format localFormat = format;
    if (false == localFormat.isBackgroundColorSet())
    {
        localFormat.backgroundColor( backgroundColor_ );
    }

    switch (format.justification())
    {
        case Justification::RIGHT:
            {
                const uint16_t textwidth = localFormat.font().getStringWidth( string );
                const uint16_t x = ((coords.x + 1U) >= textwidth) ? (coords.x + 1U - textwidth) : 0;
                driver_.putString( string, {x, coords.y}, localFormat );
            }
            break;
        case Justification::CENTER:
            {
                const uint16_t distanceFromMiddle = localFormat.font().getStringWidth( string ) / 2;
                const uint16_t x = (coords.x >= distanceFromMiddle) ? (coords.x - distanceFromMiddle) : 0;
                driver_.putString( string, {x, coords.y}, localFormat );
            }
            break;
        case Justification::LEFT:
        default:
            driver_.putString( string, coords, localFormat );
            break;
    }
}

void Lcd::print( const etl::string_view& string, uint8_t y, const Format& format )
{
    print( string, {calculateX( format.justification() ), y}, format );
}

void Lcd::setBacklightIntensity( const uint8_t intensity )
{
    backlight_.setIntensity( intensity );
}

uint8_t Lcd::maximumBacklightIntensity() const
{
    return 64; // TODO(unknown): fix
}

uint16_t Lcd::line( uint8_t lineNumber ) const
{
    return lineNumber * 8;
}

void Lcd::release()
{
}

uint16_t Lcd::calculateX( const Justification justification )
{
    uint16_t x = 0;
    switch (justification)
    {
        case Justification::LEFT:
            x = 0;
            break;
        case Justification::RIGHT:
            x = driver_.width() - 1;
            break;
        case Justification::CENTER:
            x = driver_.width() / 2;
            break;
    }

    return x;
}

}  // namespace lcd
