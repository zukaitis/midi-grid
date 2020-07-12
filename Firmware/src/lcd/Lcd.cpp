#include "lcd/Lcd.h"

#include "lcd/Parameters.h"
#include "lcd/DriverInterface.h"

#include <cmath>

namespace lcd
{

Lcd::Lcd( DriverInterface* driver, hardware::lcd::BacklightDriverInterface* backlightDriver ) :
    backlight_( backlightDriver ),
    driver_( *driver ),
    image_( &driver_ ),
    shapes_( &image_ ),
    text_( &image_ )
{
}

Lcd::~Lcd() = default;

void Lcd::initialize()
{
    driver_.initialize();
    backlight_.initialize();
}

BacklightInterface& Lcd::backlight()
{
    return backlight_;
}

ImageInterface& Lcd::image()
{
    return image_;
}

ShapesInterface& Lcd::shapes()
{
    return shapes_;
}

TextInterface& Lcd::text()
{
    return text_;
}

uint16_t Lcd::width()
{
    return parameters::width;
}

uint16_t Lcd::height()
{
    return parameters::height;
}

void Lcd::release()
{
}

}  // namespace lcd
