#pragma once

#include "io/lcd/84x48_mono/Pcd8544.h"
#include "io/lcd/84x48_mono/Lcd.h"
#include "io/lcd/backlight/Backlight.h"

namespace lcd
{

class LcdContainer
{
public:
    LcdContainer( hardware::lcd::DriverInterface& driver, hardware::lcd::BacklightDriverInterface& backlightDriver );
    virtual ~LcdContainer();

    Lcd& getLcd();

private:
    Backlight backlight_;
    Pcd8544 pcd8544_;
    Lcd lcd_;
};

}
