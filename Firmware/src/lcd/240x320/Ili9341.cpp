#include "lcd/240x320/Ili9341.h"
#include "hardware/lcd/SpiInterface.h"
#include "lcd/LcdInterface.h"
#include "ThreadConfigurations.h"

#include "lcd/font.h"

#include <freertos/ticks.hpp>
#include <sys/_stdint.h>

#include "system/System.hpp" // TODO(unknown): remove

namespace lcd
{

enum class Command : uint8_t
{
    NOP = 0x00,
    SWRESET = 0x01,
    RDDID = 0x04,
    RDDST = 0x09,
    SLPIN = 0x10,
    SLPOUT = 0x11,
    PTLON = 0x12,
    NORON = 0x13,
    RDMODE = 0x0A,
    RDMADCTL = 0x0B,
    RDPIXFMT = 0x0C,
    RDIMGFMT = 0x0A,
    RDSELFDIAG = 0x0F,
    INVOFF = 0x20,
    INVON = 0x21,
    GAMMASET = 0x26,
    DISPOFF = 0x28,
    DISPON = 0x29,
    CASET = 0x2A,
    PASET = 0x2B,
    RAMWR = 0x2C,
    RAMRD = 0x2E,
    PTLAR = 0x30,
    VSCRDEF = 0x33,
    MADCTL = 0x36,
    VSCRSADD = 0x37,
    PIXFMT = 0x3A,
    FRMCTR1 = 0xB1,
    FRMCTR2 = 0xB2,
    FRMCTR3 = 0xB3,
    INVCTR = 0xB4,
    DFUNCTR = 0xB6,
    PWCTR1 = 0xC0,
    PWCTR2 = 0xC1,
    PWCTR3 = 0xC2,
    PWCTR4 = 0xC3,
    PWCTR5 = 0xC4,
    VMCTR1 = 0xC5,
    VMCTR2 = 0xC7,
    RDID1 = 0xDA,
    RDID2 = 0xDB,
    RDID3 = 0xDC,
    RDID4 = 0xDD,
    GMCTRP1 = 0xE0,
    GMCTRN1 = 0xE1
};

Ili9341::Ili9341( hardware::lcd::SpiInterface* spi ):
    spi_( *spi )
{
}

Ili9341::~Ili9341()
{
}

void Ili9341::initialize()
{
    spi_.initialize();
    spi_.reset();

    spi_.writeCommand( 0xEF );
    {
        etl::array<uint8_t, 3> data = {0x03, 0x80, 0x02};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xCF );
    {
        etl::array<uint8_t, 3> data = {0x00, 0xC1, 0x30};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xED );
    {
        etl::array<uint8_t, 4> data = {0x64, 0x03, 0x12, 0x81};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xE8 );
    {
        etl::array<uint8_t, 3> data = {0x85, 0x00, 0x78};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xCB );
    {
        etl::array<uint8_t, 5> data = {0x39, 0x2C, 0x00, 0x34, 0x02};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xF7 );
    {
        etl::array<uint8_t, 1> data = {0x20};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xEA );
    {
        etl::array<uint8_t, 2> data = {0x00, 0x00};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR1) );
    {
        etl::array<uint8_t, 1> data = {0x23};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR2) );
    {
        etl::array<uint8_t, 1> data = {0x10};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR1) );
    {
        etl::array<uint8_t, 2> data = {0x3e, 0x28};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR2) );
    {
        etl::array<uint8_t, 1> data = {0x86};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    {
        etl::array<uint8_t, 1> data = {0x08}; // ILI9341_MADCTL_BGR
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::PIXFMT) );
    {
        etl::array<uint8_t, 1> data = {0x55};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::FRMCTR1) );
    {
        etl::array<uint8_t, 2> data = {0x00, 0x18};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::DFUNCTR) );
    {
        etl::array<uint8_t, 3> data = {0x08, 0x82, 0x27};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( 0xF2 );
    {
        etl::array<uint8_t, 1> data = {0x00};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::GAMMASET) );
    {
        etl::array<uint8_t, 1> data = {0x01};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRP1) );
    {
        etl::array<uint8_t, 15> data = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRN1) );
    {
        etl::array<uint8_t, 15> data = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::SLPOUT) );
    
    mcu::System::delayDirty( 150 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::DISPON) );

    mcu::System::delayDirty( 500 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    {
        etl::array<uint8_t, 1> data = {0x88}; // ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::CASET) );
    {
        etl::array<uint8_t, 4> data = {0x00, 0x08, 0x00, 0x80};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::PASET) );
    {
        etl::array<uint8_t, 4> data = {0x00, 0x08, 0x00, 0x80};
        spi_.writeData( etl::array_view<uint8_t>(data) );
    }

    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );

    for (uint16_t varpoksnis = 0; varpoksnis < 6050; varpoksnis++)
    {
        
        {
            etl::array<uint8_t, 2> data = {0x00, 0x00};
            spi_.writeData( etl::array_view<uint8_t>(data) );
        }
    }
    // clear();
}

void Ili9341::transmit()
{
    
}

void Ili9341::setCursor( const uint8_t column, const uint8_t row8Bit )
{

}

void Ili9341::clear()
{

}

void Ili9341::clearArea( const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2 )
{

}

void Ili9341::displayImage( const uint8_t x, const uint8_t y, const Image& image )
{
    
}

void Ili9341::putChar( const uint8_t x, const uint8_t y, const char c )
{
    
}

uint16_t Ili9341::width() const
{
    return width_;
}

uint16_t Ili9341::height() const
{
    return height_;
}

uint16_t Ili9341::numberOfTextLines() const
{
    return numberOfTextLines_;
}

} // namespace lcd