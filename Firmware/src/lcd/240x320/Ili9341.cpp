#include "lcd/240x320/Ili9341.h"
#include "hardware/lcd/SpiInterface.h"
#include "lcd/Format.h"
#include "lcd/LcdInterface.h"
#include "lcd/Image.h"
#include "ThreadConfigurations.h"

#include "lcd/Font.h"

#include <freertos/ticks.hpp>
#include <etl/algorithm.h>
#include <etl/absolute.h>
#include <sys/_stdint.h>

#include "system/System.hpp" // TODO(unknown): remove
#include "types/Coordinates.h"

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

Ili9341::Ili9341( hardware::lcd::SpiInterface* spi ):
    spi_( *spi ),
    pixelBuffer_(),
    pixelBufferIndex_( 0 ),
    dataBuffer_(),
    dataBufferIndex_( 0 )
{
}

Ili9341::~Ili9341() = default;

void Ili9341::initialize()
{
    spi_.initialize();
    spi_.reset();

    spi_.writeCommand( 0xEF );
    DataBuffer& buffer = assignDataBuffer();
    buffer = {0x03, 0x80, 0x02};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xCF );
    buffer = assignDataBuffer();
    buffer = {0x00, 0xC1, 0x30};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xED );
    buffer = assignDataBuffer();
    buffer = {0x64, 0x03, 0x12, 0x81};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xE8 );
    buffer = assignDataBuffer();
    buffer = {0x85, 0x00, 0x78};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xCB );
    buffer = assignDataBuffer();
    buffer = {0x39, 0x2C, 0x00, 0x34, 0x02};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xF7 );
    buffer = assignDataBuffer();
    buffer = {0x20};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xEA );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x00};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x23};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PWCTR2) );
    buffer = assignDataBuffer();
    buffer = {0x10};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x3e, 0x28};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::VMCTR2) );
    buffer = assignDataBuffer();
    buffer = {0x86};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    buffer = assignDataBuffer();
    buffer = {0x08}; // ILI9341_MADCTL_BGR
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PIXFMT) );
    buffer = assignDataBuffer();
    buffer = {0x66};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::FRMCTR1) );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x18};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::DFUNCTR) );
    buffer = assignDataBuffer();
    buffer = {0x08, 0x82, 0x27};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( 0xF2 );
    buffer = assignDataBuffer();
    buffer = {0x00};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GAMMASET) );
    buffer = assignDataBuffer();
    buffer = {0x01};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRP1) );
    buffer = assignDataBuffer();
    buffer = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::GMCTRN1) );
    buffer = assignDataBuffer();
    buffer = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::SLPOUT) );
    
    mcu::System::delayDirty( 150 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::DISPON) );

    mcu::System::delayDirty( 500 ); // TODO(unknown): remove

    spi_.writeCommand( static_cast<uint8_t>(Command::MADCTL) );
    buffer = assignDataBuffer();
    buffer = {0xA8}; // ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR
    spi_.writeData( hardware::lcd::RawDataView(buffer) );
}

void Ili9341::fill( const Pixel& color )
{
    fillArea( {0, 0}, {width_-1, height_-1}, color );
}

void Ili9341::fillArea( const Coordinates& corner1, const Coordinates& corner2, const Pixel& color )
{
    const Coordinates topLeft = {std::min(corner1.x, corner2.x), std::min(corner1.y, corner2.y)};
    const Coordinates bottomRight = {std::max(corner1.x, corner2.x), std::max(corner1.y, corner2.y)};
    const uint16_t width = bottomRight.x - topLeft.x + 1;
    const uint16_t height = bottomRight.y - topLeft.y + 1;
    const uint32_t areaSize = width * height;

    PixelBuffer& buffer = assignPixelBuffer();

    if (areaSize <= buffer.capacity())
    {
        setWorkingArea( topLeft, bottomRight );
        buffer.assign( areaSize, color );
        spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );
        spi_.writeData( PixelView(buffer) );
    }
    else
    {
        // interlacing
        buffer.assign( width, color );
        for (uint8_t i = 0; i < 2; i++) // loop twice
        {
            for ( uint16_t y = topLeft.y + i; y <= bottomRight.y; y += 2)
            {
                setWorkingArea( {topLeft.x, y}, {bottomRight.x, y} );
                spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );
                spi_.writeData( PixelView(buffer) );
            }
        }
    }
}

void Ili9341::displayImage( const uint8_t x, const uint8_t y, const ImageLegacy& image )
{
}

void Ili9341::putImage( const Coordinates& coords, const Image& image, const ImageColors& colors )
{
    const uint16_t width = image.getWidth();
    const uint16_t height = image.getHeight();
    const uint8_t bytesPerColumn = (height + 7) / 8;

    setWorkingArea( coords, { static_cast<uint16_t>(coords.x + width - 1),
        static_cast<uint16_t>(coords.y + height - 1) } );

    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );

    PixelBuffer& buffer = assignPixelBuffer();
    uint16_t x = 0;
    uint16_t y = 0;

    while (x < width)
    {
        const uint8_t byte = image.getData().at( x * bytesPerColumn + y / 8U );
        const bool pixelActive = ((static_cast<uint8_t>(byte >> (y % 8U)) & 0x01U) != 0U);
        buffer.emplace_back( (pixelActive) ? colors.image : colors.background );

        y++;
        if (y == height)
        {
            y = 0;
            x++;
        }

        if ((buffer.full()) || (x == width))
        {
            spi_.writeData( PixelView( buffer ) );
            buffer = assignPixelBuffer();
        }
    }
}

void Ili9341::putString( const etl::string_view& string, const Coordinates& coords )
{
    putString( string, coords, Format() );
}

// @Robert C. Martin - forgive me for this method pls
void Ili9341::putString( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    auto limitX = static_cast<uint16_t>(coords.x + format.font().getStringWidth( string ) );
    if (limitX > width_)
    {
        limitX = width_;
    }

    const Pixel textColor = format.textColor();
    const Pixel backgroundColor = format.backgroundColor();
    const uint16_t height = format.font().getHeight();
    const uint8_t bytesPerColumn = (height + 7) / 8;

    setWorkingArea( coords, { limitX, static_cast<uint16_t>(coords.y + height - 1) });

    uint16_t charIndex = 0;
    uint16_t x = 0;
    uint16_t y = 0;
    const char c = string.at( charIndex );
    Font::GlyphView glyph = format.font().getGlyph( c );
    uint16_t width = format.font().getCharWidth( c );
    bool drawGlyph = true;

    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );

    PixelBuffer& buffer = assignPixelBuffer();
    while (charIndex < string.length())
    {
        if (drawGlyph)
        {
            const uint8_t byte = glyph.at( x * bytesPerColumn + y / 8U );
            const bool pixelActive = ((static_cast<uint8_t>(byte >> (y % 8U)) & 0x01U) != 0U);
            buffer.emplace_back( (pixelActive) ? textColor : backgroundColor );
        }
        else  // draw space between glyphs
        {
            buffer.emplace_back( backgroundColor );
        }

        y++;
        if (y == height)
        {
            y = 0;
            x++;
            if (x == width)
            {
                x = 0;
                drawGlyph = !drawGlyph;
                if (drawGlyph)
                {
                    const char c = string.at( charIndex );
                    glyph = format.font().getGlyph( c );
                    width = format.font().getCharWidth( c );
                }
                else
                {
                    width = format.font().getLetterSpacingWidth();
                    charIndex++;
                }
            }
        }

        if ((buffer.full()) || charIndex == string.length())
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
    if (topLeft <= bottomRight)
    {
        spi_.writeCommand( static_cast<uint8_t>(Command::CASET) );
        DataBuffer& buffer = assignDataBuffer();
        buffer = {
            static_cast<uint8_t>(topLeft.y / 0x100U),
            static_cast<uint8_t>(topLeft.y % 0x100U),
            static_cast<uint8_t>(bottomRight.y / 0x100U),
            static_cast<uint8_t>(bottomRight.y % 0x100U) };
        spi_.writeData( hardware::lcd::RawDataView(buffer) );

        spi_.writeCommand( static_cast<uint8_t>(Command::PASET) );
        buffer = assignDataBuffer();
        buffer = {
            static_cast<uint8_t>(topLeft.x / 0x100U),
            static_cast<uint8_t>(topLeft.x % 0x100U),
            static_cast<uint8_t>(bottomRight.x / 0x100U),
            static_cast<uint8_t>(bottomRight.x % 0x100U) };
        spi_.writeData( hardware::lcd::RawDataView(buffer) );
    }
}

Ili9341::PixelBuffer& Ili9341::assignPixelBuffer()
{
    pixelBufferIndex_ = (pixelBufferIndex_ + 1) % pixelBuffer_.size();
    pixelBuffer_.at( pixelBufferIndex_ ).clear();
    return pixelBuffer_.at( pixelBufferIndex_ );
}

Ili9341::DataBuffer& Ili9341::assignDataBuffer()
{
    dataBufferIndex_ = (dataBufferIndex_ + 1) % dataBuffer_.size();
    dataBuffer_.at( dataBufferIndex_ ).clear();
    return dataBuffer_.at( dataBufferIndex_ );
}

} // namespace lcd
