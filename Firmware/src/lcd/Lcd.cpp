#include "lcd/Lcd.h"

#include "lcd/progressArc.h"
#include "lcd/font.h"

#include "lcd/DriverInterface.h"
#include "lcd/backlight/BacklightInterface.h"

#include <cstring>
#include <cmath>

namespace lcd
{

static const Image digitBig[10] = {
        { DIGITS_BIG[0], 12, 16 }, { DIGITS_BIG[1], 12, 16 }, { DIGITS_BIG[2], 12, 16 }, { DIGITS_BIG[3], 12, 16 },
        { DIGITS_BIG[4], 12, 16 }, { DIGITS_BIG[5], 12, 16 }, { DIGITS_BIG[6], 12, 16 }, { DIGITS_BIG[7], 12, 16 },
        { DIGITS_BIG[8], 12, 16 }, { DIGITS_BIG[9], 12, 16 }
};

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
    driver_.clearArea( x1, y1, x1, x2 );
}

void Lcd::displayImage( const uint8_t x, const uint8_t y, const Image image )
{
    driver_.displayImage( x, y, image );
}

void Lcd::displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position )
{
    const Image arc = { progressArcArray[position], 38, 32 };
    displayImage( x, y, arc );
}

void Lcd::initialize()
{
    driver_.initialize();
    backlight_.initialize();
}

void Lcd::putString( const char* string, uint8_t x, const uint8_t y )
{
    if (y < driver_.height()) // width_ is checked in putChar
    {
        while (*string)
        {
            driver_.putChar( x, y, *string++ );
            x += 6;
        }
    }
}

void Lcd::print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification )
{
    uint8_t textwidth = strlen( string ) * FONT_WIDTH;

    switch (justification)
    {
        case Justification::RIGHT:
            if (textwidth < x)
            {
                putString( string, (x - textwidth), y );
            }
            break;
        case Justification::CENTER:
            textwidth = textwidth / 2;
            if ((textwidth <= x) && (textwidth <= (driver_.width() - x)))
            {
                putString( string, (x - textwidth), y );
            }
            break;
        case Justification::LEFT:
        default:
            if (textwidth < (driver_.width() - x))
            {
                putString( string, x, y );
            }
            break;
    }
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

void Lcd::setBacklightIntensity( const uint8_t intensity )
{
    backlight_.setIntensity( intensity );
}

uint16_t Lcd::right() const
{
    return driver_.width() - 1;
}

uint8_t Lcd::maximumBacklightIntensity() const
{
    return 64; // TODO: fix
}

} // namespace
