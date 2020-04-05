#include "lcd/84x48_mono/LcdContainer.h"

namespace lcd
{

LcdContainer::LcdContainer( hardware::lcd::SpiInterface* spi, hardware::lcd::BacklightDriverInterface* backlightDriver ):
    backlight_( Backlight( *backlightDriver ) ),
    pcd8544_( Pcd8544( *spi ) ),
    lcd_( Lcd( pcd8544_, backlight_)),
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
