#pragma once

#include "lcd/backlight/BacklightInterface.h"
#include "lcd/image/ImageInterface.h"
#include "lcd/shapes/ShapesInterface.h"
#include "lcd/text/TextInterface.h"

#include "types/Coordinates.h"
#include <cstdint>
#include <etl/string_view.h>
#include <etl/to_string.h>

namespace lcd
{

class Font;
class ImageMono;

namespace image
{
extern const ImageMono& circle_23x23;
extern const ImageMono& circle_empty_23x23;
extern const ImageMono& down_arrow_9x23;
extern const ImageMono& midi_23x23;
extern const ImageMono& triangle_21x23;
extern const ImageMono& up_arrow_9x23;
extern const ImageMono& usb_41x23;
extern const ImageMono& usb_240x107;
}

struct ImageLegacy
{
    const uint8_t* data;
    uint8_t width;
    uint8_t height;
};

class LcdInterface
{
public:
    virtual ~LcdInterface() = default;

    virtual void initialize() = 0;

    virtual ShapesInterface& shapes() = 0;
    virtual TextInterface& text() = 0;
    virtual ImageInterface& image() = 0;
    virtual BacklightInterface& backlight() = 0;

    virtual uint16_t width() = 0;
    virtual uint16_t height() = 0;

    virtual void release() = 0;
};

}  // namespace lcd
