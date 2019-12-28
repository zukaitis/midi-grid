#include "io/lcd/84x48_mono/Pcd8544.h"
#include "hardware/lcd/DriverInterface.h"

namespace lcd
{

Pcd8544::Pcd8544( hardware::lcd::DriverInterface& driver ):
    driver_( driver )
{
}

Pcd8544::~Pcd8544()
{
}

void Pcd8544::initialize()
{
    driver_.initialize();
    driver_.reset();
    driver_.writeCommand( 0x21 ); // LCD extended commands.
    driver_.writeCommand( 0xB8 ); // set LCD Vop(Contrast).
    driver_.writeCommand( 0x04 ); // set temp coefficent.
    driver_.writeCommand( 0x15 ); // LCD bias mode 1:65
    driver_.writeCommand( 0x20 ); // LCD basic commands.
    driver_.writeCommand( 0x0C ); // LCD normal.
}

void Pcd8544::transmit( const uint8_t& data, const uint32_t size )
{
    setCursor( 0, 0 );
    driver_.writeData( data, size );
}

void Pcd8544::setCursor( const uint8_t column, const uint8_t row8Bit )
{
    driver_.writeCommand( 0x80 | column );
    driver_.writeCommand( 0x40 | row8Bit );
}

} // namespace lcd
