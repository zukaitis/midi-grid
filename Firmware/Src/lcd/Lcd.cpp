#include "lcd/Lcd.h"
#include "system/Time.h"

#include "lcd/font.h"
#include "lcd/progressArc.h"

#include <string.h>
#include <math.h>

namespace lcd
{

static const Image digitBig[10] = {
        { DIGITS_BIG[0], 24, 12, 16 }, { DIGITS_BIG[1], 24, 12, 16 }, { DIGITS_BIG[2], 24, 12, 16 }, { DIGITS_BIG[3], 24, 12, 16 },
        { DIGITS_BIG[4], 24, 12, 16 }, { DIGITS_BIG[5], 24, 12, 16 }, { DIGITS_BIG[6], 24, 12, 16 }, { DIGITS_BIG[7], 24, 12, 16 },
        { DIGITS_BIG[8], 24, 12, 16 }, { DIGITS_BIG[9], 24, 12, 16 }
};

Lcd::Lcd( mcu::Time& time ) :
        numberOfProgressArcPositions( NUMBER_OF_ARC_POSITIONS ),
        backlight_( Backlight() ),
        lcdDriver_( LcdDriver() ),
        time_( time ),
        appointedBacklightIntensity_( 0 ),
        currentBacklightIntensity_( 0 ),
        updateRequired_( false )
{
    for (uint8_t line = 0; line < numberOfLines; line++)
    {
        for (uint8_t x = 0; x < width; x++)
        {
            lcdBuffer_[line][x] = 0x00;
        }
    }

}

Lcd::~Lcd()
{
}

void Lcd::clear()
{
    memset( &lcdBuffer_[0][0], 0x00, lcdDriver_.bufferSize );
    updateRequired_ = true;
}

void Lcd::clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 )
{
    for (uint8_t j = 0; j <= ((y2-y1)/8); j++)
    {
        for (uint8_t i = 0; i < (x2-x1+1); i++)
        {
            if ((x1+i) >= width)
            {
                break;
            }
            else
            {
                lcdBuffer_[j+y1/8][x1+i] &= ~(0xFF << (y1 % 8));
                if (((j*8 + y1) < (height - 8)) && (0 != (y1 % 8)))
                {
                    lcdBuffer_[j+y1/8+1][x1+i] &= ~(0xFF >> (8 - y1 % 8));
                }
            }
        }
    }
    updateRequired_ = true;
}

void Lcd::displayImage( const uint8_t x, const uint8_t y, const Image image )
{
    for (uint8_t j = 0; j < (image.height/8); j++)
    {
        for (uint8_t i = 0; i < image.width; i++)
        {
            if ((x+i) >= width)
            {
                break;
            }
            else
            {
                lcdBuffer_[j+y/8][x+i] &= ~(0xFF << (y % 8));
                lcdBuffer_[j+y/8][x+i] |= image.image[j*image.width + i] << (y % 8);

                if (((j*8 + y) < (height - 8)) && (0 != (y % 8)))
                {
                    lcdBuffer_[j+y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                    lcdBuffer_[j+y/8+1][x+i] |= image.image[j*image.width + i] >> (8 - y % 8);
                }
            }
        }
    }
    updateRequired_ = true;
}

void Lcd::displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position )
{
    const Image arc = { progressArcArray[position], 152, 38, 32 };
    displayImage( x, y, arc );
}

void Lcd::initialize()
{
    lcdDriver_.initialize();
    backlight_.initialize();
    clear();
}

void Lcd::print( const char* string, uint8_t x, const uint8_t y )
{
    if (y < height) // width is checked in putChar
    {
        while (*string)
        {
            putChar( x, y, *string++ );
            x += 6;
        }
    }
    updateRequired_ = true;
}

void Lcd::print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification )
{
    uint8_t textWidth = strlen( string ) * FONT_WIDTH;

    switch (justification)
    {
        case Justification_RIGHT:
            if (textWidth < x)
            {
                print( string, (x - textWidth), y );
            }
            break;
        case Justification_CENTER:
            textWidth = textWidth / 2;
            if ((textWidth <= x) && (textWidth <= (width - x)))
            {
                print( string, (x - textWidth), y );
            }
            break;
        case Justification_LEFT:
        default:
            if (textWidth < (width - x))
            {
                print( string, x, y );
            }
            break;
    }
}

void Lcd::printNumberInBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits )
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

    uint8_t textWidth = numberOfDigits * digitBig[0].width;

    switch (justification)
    {
        case Justification_RIGHT:
            if (textWidth < x)
            {
                printNumberInBigDigits( number, (x - textWidth), y, numberOfDigits );
            }
            break;
        case Justification_CENTER:
            textWidth = textWidth / 2;
            if ((textWidth <= x) && (textWidth <= (width - x)))
            {
                printNumberInBigDigits( number, (x - textWidth), y, numberOfDigits );
            }
            break;
        case Justification_LEFT:
        default:
            if (textWidth < (width - x))
            {
                printNumberInBigDigits( number, x, y, numberOfDigits );
            }
            break;
    }
}

void Lcd::refresh()
{
    static uint32_t refreshCheckTime = 0;

    if (time_.getSystemTick() >= refreshCheckTime)
    {
        if (updateRequired_)
        {
            lcdDriver_.transmit( &lcdBuffer_[0][0] );
            updateRequired_ = false;
        }

        if (currentBacklightIntensity_ != appointedBacklightIntensity_)
        {
            if (currentBacklightIntensity_ > appointedBacklightIntensity_)
            {
                backlight_.setIntensity( --currentBacklightIntensity_ );
            }
            else
            {
                backlight_.setIntensity( ++currentBacklightIntensity_ );
            }
        }

        refreshCheckTime = time_.getSystemTick() + 20; // check every 20ms
    }
}

void Lcd::setBacklightIntensity( const uint8_t intensity )
{
    appointedBacklightIntensity_ = intensity;
}

void Lcd::putChar( const uint8_t x, const uint8_t y, const char c )
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if ((x+i) >= width)
        {
            break;
        }
        else
        {
            lcdBuffer_[y/8][x+i] &= ~(0xFF << (y % 8));
            lcdBuffer_[y/8][x+i] |= ASCII[c-0x20][i] << (y % 8);

            if ((y < (height - FONT_HEIGHT)) && (0 != (y % 8)))
            {
                lcdBuffer_[y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                lcdBuffer_[y/8+1][x+i] |= ASCII[c-0x20][i] >> (8 - y % 8);
            }
        }
    }
}

} // namespace
