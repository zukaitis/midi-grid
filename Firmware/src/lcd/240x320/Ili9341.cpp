#include "lcd/240x320/Ili9341.h"
#include "hardware/lcd/SpiInterface.h"
#include "lcd/LcdInterface.h"
#include "ThreadConfigurations.h"

#include "lcd/font.h"

#include <freertos/ticks.hpp>
#include <etl/algorithm.h>
#include <etl/absolute.h>

#include "system/System.hpp" // TODO(unknown): remove

namespace lcd
{

static const uint8_t asciiOffset = 0x20; // TODO(unknown): delet

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

namespace color
{
    static const Pixel BLACK = {0, 0, 0};
    static const Pixel RED = {255, 0, 0};
    static const Pixel GREEN = {0, 255, 0};
    static const Pixel BLUE = {0, 0, 255};
    static const Pixel YELLOW = {255, 255, 0};
    static const Pixel MAGENTA = {255, 0, 255};
    static const Pixel CYAN = {0, 255, 255};
    static const Pixel WHITE = {255, 255, 255};
    static const Pixel ORANGE = {255, 125, 0};
    static const Pixel SPRING_GREEN = {125, 255, 0};
    static const Pixel TURQUOISE = {0, 255, 125};
    static const Pixel OCEAN = {0, 125, 255};
    static const Pixel VIOLET = {125, 0, 255};
    static const Pixel RASPBERRY = {255, 0, 125};
}

Ili9341::Ili9341( hardware::lcd::SpiInterface* spi ):
    spi_( *spi ),
    pixelBuffer_(),
    pixelBufferIndex_( 0 ),
    dataBuffer_(),
    dataBufferIndex_( 0 )
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
    DataBuffer& buffer = assignDataBuffer();
    buffer = {0x03, 0x80, 0x02};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xCF );
    buffer = assignDataBuffer();
    buffer = {0x00, 0xC1, 0x30};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xED );
    buffer = assignDataBuffer();
    buffer = {0x64, 0x03, 0x12, 0x81};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xE8 );
    buffer = assignDataBuffer();
    buffer = {0x85, 0x00, 0x78};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xCB );
    buffer = assignDataBuffer();
    buffer = {0x39, 0x2C, 0x00, 0x34, 0x02};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xF7 );
    buffer = assignDataBuffer();
    buffer = {0x20};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xEA );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x00};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x23};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR2) );
    buffer = assignDataBuffer();
    buffer = {0x10};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x3e, 0x28};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR2) );
    buffer = assignDataBuffer();
    buffer = {0x86};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    buffer = assignDataBuffer();
    buffer = {0x08}; // ILI9341_MADCTL_BGR
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PIXFMT) );
    buffer = assignDataBuffer();
    buffer = {0x66};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::FRMCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x18};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::DFUNCTR) );
    buffer = assignDataBuffer();
    buffer = {0x08, 0x82, 0x27};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( 0xF2 );
    buffer = assignDataBuffer();
    buffer = {0x00};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GAMMASET) );
    buffer = assignDataBuffer();
    buffer = {0x01};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRP1) );
    buffer = assignDataBuffer();
    buffer = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRN1) );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
    spi_.writeData( RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::SLPOUT) );
    
    mcu::System::delayDirty( 150 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::DISPON) );

    mcu::System::delayDirty( 500 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    buffer = assignDataBuffer();
    buffer = {0xA8}; // ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR
    spi_.writeData( RawDataView(buffer) );

    clear();
}

void Ili9341::clear()
{
    clearArea( {0, 0}, {width_, height_} );
}

void Ili9341::clearArea( const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2 )
{
    // deprecated
    clearArea( {x1, y1}, {x2, y2} );
}

void Ili9341::clearArea( const Coordinates& corner1, const Coordinates& corner2 )
{
    fillArea( corner1, corner2, color::BLACK );
}

void Ili9341::fillArea( const Coordinates& corner1, const Coordinates& corner2, const Pixel& color )
{
    setWorkingArea(
        {std::min(corner1.x, corner2.x), std::min(corner1.y, corner2.y)},
        {std::max(corner1.x, corner2.x), std::max(corner1.y, corner2.y)} );

    const uint32_t areaSize = etl::absolute(corner1.x - corner2.x) * etl::absolute(corner1.y - corner2.y);

    PixelBuffer& buffer = assignPixelBuffer();
    buffer.assign( buffer.capacity(), color );
    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );
    for (uint32_t i = 0; i < ((areaSize / buffer.size()) + 1); i++)
    {
        spi_.writeData( PixelView(buffer) );
    }
}

void Ili9341::displayImage( const uint8_t x, const uint8_t y, const Image& image )
{
    
}

void Ili9341::putString( const etl::string_view& string, const Coordinates& coords )
{
    auto limitX = static_cast<uint16_t>(coords.x + string.length()*FONT_WIDTH);
    if (limitX > width_)
    {
        limitX = width_;
    }

    setWorkingArea( coords, { limitX, static_cast<uint16_t>(coords.y + FONT_HEIGHT - 1) });

    const Pixel textColor = color::WHITE;
    const Pixel backgroundColor = color::BLACK;

    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );

    uint16_t x = coords.x;
    uint16_t y = 0;
    PixelBuffer& buffer = assignPixelBuffer();
    while (x < limitX)
    {
        const char c = string.at( (x - coords.x) / FONT_WIDTH );
        const uint8_t column = ASCII.at( c - asciiOffset ).at( (x - coords.x) % FONT_WIDTH );
        const bool textPixel = (((column >> y) & 0x01U) != 0U);
        buffer.emplace_back( (textPixel) ? textColor : backgroundColor );

        y++;
        if (y == FONT_HEIGHT)
        {
            y = 0;
            x++;
        }

        if ((buffer.full()) || (x == limitX))
        {
            spi_.writeData( PixelView( buffer ) );
            buffer = assignPixelBuffer();
        }
    }
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

void Ili9341::setWorkingArea( const Coordinates& topLeft, const Coordinates& bottomRight )
{
    if (topLeft < bottomRight)
    {
        spi_.writeCommand( static_cast<uint8_t>(Command::CASET) );
        const etl::array<uint8_t, 4> limitsY = {
            static_cast<uint8_t>(topLeft.y / 0x100U),
            static_cast<uint8_t>(topLeft.y % 0x100U),
            static_cast<uint8_t>(bottomRight.y / 0x100U),
            static_cast<uint8_t>(bottomRight.y % 0x100U) };
        spi_.writeData( etl::array_view<const uint8_t>(limitsY) );

        spi_.writeCommand( static_cast<uint8_t>(Command::PASET) );
        const etl::array<uint8_t, 4> limitsX = {
            static_cast<uint8_t>(topLeft.x / 0x100U),
            static_cast<uint8_t>(topLeft.x % 0x100U),
            static_cast<uint8_t>(bottomRight.x / 0x100U),
            static_cast<uint8_t>(bottomRight.x % 0x100U) };
        spi_.writeData( etl::array_view<const uint8_t>(limitsX) );
    }
}

Ili9341::PixelBuffer& Ili9341::assignPixelBuffer()
{
    pixelBufferIndex_ = (pixelBufferIndex_ + 1) / pixelBuffer_.size();
    pixelBuffer_.at( pixelBufferIndex_ ).clear();
    return pixelBuffer_.at( pixelBufferIndex_ );
}

Ili9341::DataBuffer& Ili9341::assignDataBuffer()
{
    dataBufferIndex_ = (dataBufferIndex_ + 1) / dataBuffer_.size();
    dataBuffer_.at( dataBufferIndex_ ).clear();
    return dataBuffer_.at( dataBufferIndex_ );
}


} // namespace lcd