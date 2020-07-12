#include "lcd/Ili9341.h"
#include "hardware/lcd/SpiInterface.h"

#include <etl/algorithm.h>
#include <etl/absolute.h>

#include "system/System.hpp" // TODO(unknown): remove
#include "types/Coordinates.h"

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
    spi_( *spi ),
    dataBuffer_(),
    dataBufferIndex_( 0 ),
    buffer_(),
    refreshAreaTopLeft_( {parameters::width, parameters::height} ),
    refreshAreaBottomRight_( {0, 0} )
{
}

Ili9341::~Ili9341() = default;

void Ili9341::initialize()
{
    spi_.initialize();
    spi_.setBulkTransferStartAddressIncrement( parameters::width );
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
    buffer = {0xA8}; // ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR
    spi_.writeData( hardware::lcd::RawDataView(buffer) );

    spi_.writeCommand( static_cast<uint8_t>(Command::PIXFMT) );
    buffer = assignDataBuffer();
    buffer = {0x55};
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

    spi_.writeCommand( static_cast<uint8_t>(Command::PIXFMT) );
    buffer = assignDataBuffer();
    buffer = {0x55};
    spi_.writeData( hardware::lcd::RawDataView(buffer) );
}

void Ili9341::fill( const Color& color )
{
    // TODO(): implement this
}

void Ili9341::putPixel( const Coordinates& coords, const Color& color )
{
    const Pixel pixel( color );
    if (pixel != buffer_.at(coords.y).at(coords.x))
    {
        buffer_.at(coords.y).at(coords.x) = pixel;
        updateRefreshArea( coords );
    }
}

void Ili9341::flush()
{
    if (refreshAreaTopLeft_ <= refreshAreaBottomRight_)
    {
        const uint16_t refreshAreaWidth = refreshAreaBottomRight_.x - refreshAreaTopLeft_.x + 1U;

        if (refreshAreaWidth < 80)
        {
            refresh();
        }
        else
        {
            refreshInterlaced();
        }
    }

    resetRefreshArea();
}

void Ili9341::refresh()
{
    const uint16_t areaWidth = refreshAreaBottomRight_.x - refreshAreaTopLeft_.x + 1U;
    const uint16_t areaHeight = refreshAreaBottomRight_.y - refreshAreaTopLeft_.y + 1U;

    setWorkingArea( refreshAreaTopLeft_, refreshAreaBottomRight_ );
    spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );
    spi_.writeData( &buffer_.at(refreshAreaTopLeft_.y).at(refreshAreaTopLeft_.x), areaWidth * sizeof(Pixel), areaHeight * sizeof(Pixel) );
}

void Ili9341::refreshInterlaced()
{
    const uint16_t areaWidth = refreshAreaBottomRight_.x - refreshAreaTopLeft_.x + 1U;

    setWorkingLimitsX( refreshAreaTopLeft_.x, refreshAreaBottomRight_.x );
    for (uint8_t i = 0; i < 2; i++)  // loop twice
    {
        for (uint16_t y = refreshAreaTopLeft_.y + i; y <= refreshAreaBottomRight_.y; y += 2)
        {
            setWorkingLimitsY( y, y ); // transmit one line at a time
            spi_.writeCommand( static_cast<uint8_t>(Command::RAMWR) );
            spi_.writeData( &buffer_.at(y).at(refreshAreaTopLeft_.x), areaWidth * sizeof(Pixel) );
        }
    }
}

void Ili9341::updateRefreshArea( const Coordinates& coords )
{
    refreshAreaTopLeft_.x = std::min(coords.x, refreshAreaTopLeft_.x);
    refreshAreaBottomRight_.x = std::max(coords.x, refreshAreaBottomRight_.x);
    refreshAreaTopLeft_.y = std::min(coords.y, refreshAreaTopLeft_.y);
    refreshAreaBottomRight_.y = std::max(coords.y, refreshAreaBottomRight_.y);
}

void Ili9341::resetRefreshArea()
{
    refreshAreaTopLeft_ = {parameters::width, parameters::height};
    refreshAreaBottomRight_ = {0, 0};
}

void Ili9341::setWorkingArea( const Coordinates& topLeft, const Coordinates& bottomRight )
{
    setWorkingLimitsX( topLeft.x, bottomRight.x );
    setWorkingLimitsX( topLeft.y, bottomRight.y );
}

void Ili9341::setWorkingLimitsX( const uint16_t left, const uint16_t right )
{
    if (left <= right)
    {
        spi_.writeCommand( static_cast<uint8_t>(Command::PASET) );
        DataBuffer& buffer = assignDataBuffer();
        buffer = {
            static_cast<uint8_t>(left / 0x100U),
            static_cast<uint8_t>(left % 0x100U),
            static_cast<uint8_t>(right / 0x100U),
            static_cast<uint8_t>(right % 0x100U) };
        spi_.writeData( hardware::lcd::RawDataView(buffer) );
    }
}

void Ili9341::setWorkingLimitsY( const uint16_t top, const uint16_t bottom )
{
    if (top <= bottom)
    {
        spi_.writeCommand( static_cast<uint8_t>(Command::CASET) );
        DataBuffer& buffer = assignDataBuffer();
        buffer = {
            static_cast<uint8_t>(top / 0x100U),
            static_cast<uint8_t>(top % 0x100U),
            static_cast<uint8_t>(bottom / 0x100U),
            static_cast<uint8_t>(bottom % 0x100U) };
        spi_.writeData( hardware::lcd::RawDataView(buffer) );
    }
}

Ili9341::DataBuffer& Ili9341::assignDataBuffer()
{
    dataBufferIndex_ = (dataBufferIndex_ + 1) % dataBuffer_.size();
    dataBuffer_.at( dataBufferIndex_ ).clear();
    return dataBuffer_.at( dataBufferIndex_ );
}

} // namespace lcd
