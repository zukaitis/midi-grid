#pragma once

#include "lcd/LcdInterface.h"

#include "lcd/backlight/Backlight.h"
#include "lcd/image/Image.h"
#include "lcd/shapes/Shapes.h"
#include "lcd/text/Text.h"

namespace lcd
{

class DriverInterface;

class Lcd: public LcdInterface
{
public:
    Lcd( DriverInterface* driver, hardware::lcd::BacklightDriverInterface* backlightDriver );
    ~Lcd() override;

    void initialize() override;

    BacklightInterface& backlight() override;
    ImageInterface& image() override;
    ShapesInterface& shapes() override;
    TextInterface& text() override;

    uint16_t width() override;
    uint16_t height() override;

    void release() override;

private:
    DriverInterface& driver_;
    Backlight backlight_;
    Image image_;
    Shapes shapes_;
    Text text_;
};

}  // namespace lcd
