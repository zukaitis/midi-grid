#pragma once

#include "types/Color.h"
#include <etl/array.h>

struct Coordinates;

namespace grid
{

static const uint8_t NUMBER_OF_FLASHING_COLORS = 2;
typedef etl::array<Color, NUMBER_OF_FLASHING_COLORS> FlashingColors;

class FlashingLedsInterface
{
public:
    virtual ~FlashingLedsInterface() {};

    virtual void add( const Coordinates& coordinates, const FlashingColors& colors ) = 0;
    virtual void remove( const Coordinates& coordinates ) = 0;
    virtual void removeAll() = 0;
};

}
