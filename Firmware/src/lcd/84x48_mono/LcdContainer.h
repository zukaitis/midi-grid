#pragma once

#include "lcd/Lcd.h"
#include "lcd/PriorityManager.h"
#include "lcd/84x48_mono/Pcd8544.h"
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
    Backlight backlight_;
    Pcd8544 pcd8544_;
    Lcd lcd_;
    PriorityManager managerPriorityLow_;
    PriorityManager managerPriorityHigh_;
};

}
