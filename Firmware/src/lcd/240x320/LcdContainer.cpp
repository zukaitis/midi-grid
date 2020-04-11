#include "lcd/240x320/LcdContainer.h"

namespace lcd
{

LcdContainer::LcdContainer( hardware::lcd::SpiInterface* spi, hardware::lcd::BacklightDriverInterface* backlightDriver ):
    backlight_( Backlight( *backlightDriver ) ),
    ili9341_( Ili9341( spi ) ),
    lcd_( Lcd( ili9341_, backlight_)),
    managerPriorityLow_( &lcd_, Priority::LOW ),
    managerPriorityHigh_( &lcd_, Priority::HIGH )
{
}

LcdContainer::~LcdContainer()
{
}

LcdInterface& LcdContainer::getLcdLowPriority()
{
    return managerPriorityLow_;
}

LcdInterface& LcdContainer::getLcdHighPriority()
{
    return managerPriorityHigh_;
}

}
