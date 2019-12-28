#include "io/lcd/84x48_mono/LcdContainer.h"

namespace lcd
{

LcdContainer::LcdContainer( hardware::lcd::DriverInterface& driver, hardware::lcd::BacklightDriverInterface& backlightDriver ):
    backlight_( Backlight( backlightDriver ) ),
    pcd8544_( Pcd8544( driver ) ),
    lcd_( Lcd( pcd8544_, backlight_))
{
}

LcdContainer::~LcdContainer()
{
}

Lcd& LcdContainer::getLcd()
{
    return lcd_;
}


}
