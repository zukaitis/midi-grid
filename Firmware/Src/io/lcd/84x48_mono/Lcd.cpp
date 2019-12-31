#include "io/lcd/84x48_mono/Lcd.h"

#include "io/lcd/font.h"
#include "io/lcd/progressArc.h"
#include "ThreadConfigurations.h"

#include "io/lcd/84x48_mono/Pcd8544Interface.h"
#include "io/lcd/backlight/BacklightInterface.h"

#include <freertos/ticks.hpp>
#include <cstring>
#include <cmath>

namespace lcd
{

static const Image digitBig[10] = {
        { DIGITS_BIG[0], 12, 16 }, { DIGITS_BIG[1], 12, 16 }, { DIGITS_BIG[2], 12, 16 }, { DIGITS_BIG[3], 12, 16 },
        { DIGITS_BIG[4], 12, 16 }, { DIGITS_BIG[5], 12, 16 }, { DIGITS_BIG[6], 12, 16 }, { DIGITS_BIG[7], 12, 16 },
        { DIGITS_BIG[8], 12, 16 }, { DIGITS_BIG[9], 12, 16 }
};

Lcd::Lcd( Pcd8544Interface& driver, BacklightInterface& backlight ) :
        Thread( "Lcd", kLcd.stackDepth, kLcd.priority ),
        backlight_( backlight ),
        driver_( driver ),
        buffer_(),
        updateRequired_()
{
}

Lcd::~Lcd()
{
}

void Lcd::Run()
{
    const TickType_t delayPeriod = freertos::Ticks::MsToTicks( 10 );
    const uint32_t bufferSize = sizeof( buffer_ ) / sizeof( buffer_[0][0] );

    updateRequired_.Take(); // block until LCD update is required
    DelayUntil( delayPeriod ); // delay, in case multiple things are to be updated one after another
    driver_.transmit( buffer_[0][0], bufferSize );
}

void Lcd::clear()
{
    buffer_ = {};
    updateRequired_.Give();
}

void Lcd::clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 )
{
    for (uint8_t j = 0; j <= ((y2-y1)/8); j++)
    {
        for (uint8_t i = 0; i < (x2-x1+1); i++)
        {
            if ((x1+i) >= width_)
            {
                break;
            }
            else
            {
                buffer_[j+y1/8][x1+i] &= ~(0xFF << (y1 % 8));
                if (((j*8 + y1) < (height_ - 8)) && (0 != (y1 % 8)))
                {
                    buffer_[j+y1/8+1][x1+i] &= ~(0xFF >> (8 - y1 % 8));
                }
            }
        }
    }
    updateRequired_.Give();
}

void Lcd::displayImage( const uint8_t x, const uint8_t y, const Image image )
{
    for (uint8_t j = 0; j < (image.height/8); j++)
    {
        for (uint8_t i = 0; i < image.width; i++)
        {
            if ((x+i) >= width_)
            {
                break;
            }
            else
            {
                buffer_[j+y/8][x+i] &= ~(0xFF << (y % 8));
                buffer_[j+y/8][x+i] |= image.data[j*image.width + i] << (y % 8);

                if (((j*8 + y) < (height_ - 8)) && (0 != (y % 8)))
                {
                    buffer_[j+y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                    buffer_[j+y/8+1][x+i] |= image.data[j*image.width + i] >> (8 - y % 8);
                }
            }
        }
    }
    updateRequired_.Give();
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
    clear();
    Start();
}

void Lcd::putString( const char* string, uint8_t x, const uint8_t y )
{
    if (y < height_) // width_ is checked in putChar
    {
        while (*string)
        {
            putChar( x, y, *string++ );
            x += 6;
        }
        updateRequired_.Give();
    }
}

void Lcd::print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification )
{
    uint8_t textwidth_ = strlen( string ) * FONT_WIDTH;

    switch (justification)
    {
        case Justification::RIGHT:
            if (textwidth_ < x)
            {
                putString( string, (x - textwidth_), y );
            }
            break;
        case Justification::CENTER:
            textwidth_ = textwidth_ / 2;
            if ((textwidth_ <= x) && (textwidth_ <= (width_ - x)))
            {
                putString( string, (x - textwidth_), y );
            }
            break;
        case Justification::LEFT:
        default:
            if (textwidth_ < (width_ - x))
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
            if ((textwidth_ <= x) && (textwidth_ <= (width_ - x)))
            {
                putBigDigits( number, (x - textwidth_), y, numberOfDigits );
            }
            break;
        case Justification::LEFT:
        default:
            if (textwidth_ < (width_ - x))
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

void Lcd::putChar( const uint8_t x, const uint8_t y, const char c )
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if ((x+i) >= width_)
        {
            break;
        }
        else
        {
            buffer_[y/8][x+i] &= ~(0xFF << (y % 8));
            buffer_[y/8][x+i] |= ASCII[c-0x20][i] << (y % 8);

            if ((y < (height_ - FONT_HEIGHT)) && (0 != (y % 8)))
            {
                buffer_[y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                buffer_[y/8+1][x+i] |= ASCII[c-0x20][i] >> (8 - y % 8);
            }
        }
    }
}

uint16_t Lcd::width() const
{
    return width_;
}

uint16_t Lcd::height() const
{
    return height_;
}

uint16_t Lcd::numberOfTextLines() const
{
    return numberOfTextLines_;
}

uint8_t Lcd::maximumBacklightIntensity() const
{
    return 64; // TODO: fix
}

} // namespace
