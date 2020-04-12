#include "lcd/Lcd.h"

#include "lcd/progressArc.h"
#include "lcd/font.h"

#include "lcd/DriverInterface.h"
#include "lcd/backlight/BacklightInterface.h"

#include <cmath>

namespace lcd
{

static const etl::array<Image, 10> digitBig = {{
        { &DIGITS_BIG[0][0], 12, 16 }, { &DIGITS_BIG[1][0], 12, 16 }, { &DIGITS_BIG[2][0], 12, 16 }, { &DIGITS_BIG[3][0], 12, 16 },
        { &DIGITS_BIG[4][0], 12, 16 }, { &DIGITS_BIG[5][0], 12, 16 }, { &DIGITS_BIG[6][0], 12, 16 }, { &DIGITS_BIG[7][0], 12, 16 },
        { &DIGITS_BIG[8][0], 12, 16 }, { &DIGITS_BIG[9][0], 12, 16 }
}};

Lcd::Lcd( DriverInterface& driver, BacklightInterface& backlight ) :
    driver_( driver ),
    backlight_( backlight )
{
}

Lcd::~Lcd()
{
}

void Lcd::clear()
{
    driver_.clear();
}

void Lcd::clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 )
{
    driver_.clearArea( x1, y1, x2, y2 );
}

void Lcd::displayImage( const uint8_t x, const uint8_t y, const Image& image )
{
    driver_.displayImage( x, y, image );
}

void Lcd::displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position )
{
    const Image arc = { &progressArcArray.at(position)[0], 38, 32 };
    displayImage( x, y, arc );
}

void Lcd::initialize()
{
    driver_.initialize();
    backlight_.initialize();
}

void Lcd::putString( const etl::string_view& string, uint8_t x, const uint8_t y )
{
    if (y < driver_.height()) // width_ is checked in putChar
    {
        for (char c : string)
        {
            driver_.putChar( x, y, c );
            x += 6;
        }
    }
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

void Lcd::putBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits )
{
    uint16_t divisor = pow( 10, numberOfDigits );
    while (divisor > 1)
    {
        number %= divisor;
        divisor /= 10;
        displayImage( x, y, digitBig[number/divisor] );
        x += digitBig[0].width;
    }
}

void Lcd::printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification )
{
    uint8_t numberOfDigits = 5; // 5 digit numbers max
    uint16_t divisor = 10000;
    while (divisor > 1)
    {
        if (0 != (number / divisor))
        {
            break;
        }
        --numberOfDigits;
        divisor /= 10;
    }

    uint8_t textwidth_ = numberOfDigits * digitBig[0].width;

    switch (justification)
    {
        case Justification::RIGHT:
            if (textwidth_ < x)
            {
                putBigDigits( number, (x - textwidth_), y, numberOfDigits );
            }
            break;
        case Justification::CENTER:
            textwidth_ = textwidth_ / 2;
            if ((textwidth_ <= x) && (textwidth_ <= (driver_.width() - x)))
            {
                putBigDigits( number, (x - textwidth_), y, numberOfDigits );
            }
            break;
        case Justification::LEFT:
        default:
            if (textwidth_ < (driver_.width() - x))
            {
                putBigDigits( number, x, y, numberOfDigits );
            }
            break;
    }
}

void Lcd::printNumberInBigDigits( const uint16_t number, const uint8_t y, const Justification justification )
{
    printNumberInBigDigits( number, calculateX( justification ), y, justification );
}

void Lcd::printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y )
{
    printNumberInBigDigits( number, x, y, Justification::LEFT );
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
