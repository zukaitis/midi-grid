#include "lcd/LcdContainer.h"

namespace lcd
{

LcdContainer::LcdContainer( hardware::lcd::SpiInterface* spi, hardware::lcd::BacklightDriverInterface* backlightDriver ):
    ili9341_( spi ),
    lcd_( &ili9341_, backlightDriver ),
    managerPriorityLow_( &lcd_, Priority::LOW ),
    managerPriorityHigh_( &lcd_, Priority::HIGH )
{
}

LcdContainer::~LcdContainer() = default;

LcdInterface& LcdContainer::getLcdLowPriority()
{
    return managerPriorityLow_;
}

LcdInterface& LcdContainer::getLcdHighPriority()
{
    return managerPriorityHigh_;
}

} // namespace lcd
