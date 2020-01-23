#include "lcd/84x48_mono/Pcd8544.h"
#include "hardware/lcd/SpiInterface.h"
#include "lcd/LcdInterface.h"
#include "ThreadConfigurations.h"

#include "lcd/font.h"

#include <freertos/ticks.hpp>

namespace lcd
{

Pcd8544::Pcd8544( hardware::lcd::SpiInterface& spi ):
    Thread( "Lcd", kLcd.stackDepth, kLcd.priority ),
    spi_( spi ),
    buffer_(),
    updateRequired_( false )
{
}

Pcd8544::~Pcd8544()
{
}

void Pcd8544::Run()
{
    const TickType_t delayPeriod = freertos::Ticks::MsToTicks( 10 );
    const uint32_t bufferSize = sizeof( buffer_ ) / sizeof( buffer_[0][0] ); // TODO: remove

    updateRequired_.Take(); // block until LCD update is required
    DelayUntil( delayPeriod ); // delay, in case multiple things are to be updated one after another
    transmit();
}

void Pcd8544::initialize()
{
    spi_.initialize();
    spi_.reset();
    spi_.writeCommand( 0x21 ); // LCD extended commands.
    spi_.writeCommand( 0xB8 ); // set LCD Vop(Contrast).
    spi_.writeCommand( 0x04 ); // set temp coefficent.
    spi_.writeCommand( 0x15 ); // LCD bias mode 1:65
    spi_.writeCommand( 0x20 ); // LCD basic commands.
    spi_.writeCommand( 0x0C ); // LCD normal.

    clear();
    Start();
}

void Pcd8544::transmit()
{
    const uint32_t bufferSize = sizeof( buffer_ ) / sizeof( buffer_[0][0] );

    setCursor( 0, 0 );
    spi_.writeData( buffer_[0][0], bufferSize );
}

void Pcd8544::setCursor( const uint8_t column, const uint8_t row8Bit )
{
    spi_.writeCommand( 0x80 | column );
    spi_.writeCommand( 0x40 | row8Bit );
}

void Pcd8544::clear()
{
    buffer_ = {};
    updateRequired_.Give();
}

void Pcd8544::clearArea( const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2 )
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

void Pcd8544::displayImage( const uint8_t x, const uint8_t y, const Image& image )
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

                updateRequired_.Give();
            }
        }
    }
}

void Pcd8544::putChar( const uint8_t x, const uint8_t y, const char c )
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
            updateRequired_.Give();
        }
    }
}

uint16_t Pcd8544::width() const
{
    return width_;
}

uint16_t Pcd8544::height() const
{
    return height_;
}

uint16_t Pcd8544::numberOfTextLines() const
{
    return numberOfTextLines_;
}

} // namespace lcd
