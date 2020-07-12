#include "lcd/Pixel.h"

namespace lcd
{

static const uint8_t redShift = 3U;
static const uint8_t redMask = 0b11111000U;
static const uint8_t greenShift = 3U;
static const uint8_t greenMask = 0b11111100U;
static const uint8_t blueShift = 8U;
static const uint8_t blueMask = 0b11111000U;

Pixel::Pixel( Color color ):
    value_( ((color.red() & redMask) >> redShift) |
        ((color.green() & greenMask) << greenShift) |
        ((color.blue() & blueMask) << blueShift) )
{
}

bool Pixel::operator==(const Pixel& other) const
{
    return (this->value_ == other.value_);
};

bool Pixel::operator!=(const Pixel& other) const
{
    return !(other == *this);
};

}
