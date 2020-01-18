#pragma once

#include "lcd/84x48_mono/Pcd8544.h"
#include "lcd/Lcd.h"
#include "lcd/backlight/Backlight.h"

namespace lcd
{

class LcdContainer
{
public:
    LcdContainer( hardware::lcd::SpiInterface& spi, hardware::lcd::BacklightDriverInterface& backlightDriver );
    virtual ~LcdContainer();

    Lcd& getLcd();

private:
    Backlight backlight_;
    Pcd8544 pcd8544_;
    Lcd lcd_;
};

}
