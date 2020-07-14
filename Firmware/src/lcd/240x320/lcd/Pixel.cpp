#include "lcd/Pixel.h"

namespace lcd
{

static const uint16_t redMask = 0b11111000U;
static const uint16_t greenShiftHigh = 5U;
static const uint16_t greenMaskHigh = 0b11100000U;
static const uint16_t greenShiftLow = 11U;
static const uint16_t greenMaskLow = 0b00011100U;
static const uint16_t blueShift = 5U;
static const uint16_t blueMask = 0b11111000U;

Pixel::Pixel( Color color ):
    value_( (color.red() & redMask) |
        ((color.green() & greenMaskHigh) >> greenShiftHigh) |
        ((color.green() & greenMaskLow) << greenShiftLow) |
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
