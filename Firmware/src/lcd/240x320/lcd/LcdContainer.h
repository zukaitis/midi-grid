#pragma once

#include "lcd/Lcd.h"
#include "lcd/PriorityManager.h"
#include "lcd/Ili9341.h"
#include "lcd/backlight/Backlight.h"

namespace lcd
{

class LcdContainer
{
public:
    LcdContainer( hardware::lcd::SpiInterface* spi, hardware::lcd::BacklightDriverInterface* backlightDriver );
    virtual ~LcdContainer();

    LcdInterface& getLcdLowPriority();
    LcdInterface& getLcdHighPriority();

private:
    Ili9341 ili9341_;
    Lcd lcd_;
    PriorityManager managerPriorityLow_;
    PriorityManager managerPriorityHigh_;
};

}  // namespace lcd
