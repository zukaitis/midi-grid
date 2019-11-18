#pragma once

#include "io/grid/GridInterface.h"

#include "types/Color.h"
#include "types/Coordinates.h"

#include "thread.hpp"
#include <etl/array.h>
#include <etl/vector.h>

namespace grid
{

class LedOutput;

static const uint8_t NUMBER_OF_FLASHING_COLORS = 2;
typedef etl::array<Color, NUMBER_OF_FLASHING_COLORS> FlashingColors;
static const uint32_t FLASHING_PERIOD = 250; // 120bpm - default flashing rate

class FlashingLeds: private freertos::Thread
{
public:
    FlashingLeds( LedOutput& ledOutput );

    void add( const Coordinates& coordinates, const FlashingColors& color );
    void remove( const Coordinates& coordinates );
    void removeAll();

private:
    virtual void Run();

    struct FlashingLed
    {
        Coordinates coordinates;
        FlashingColors color;
    };

    LedOutput& ledOutput_;
    etl::vector<FlashingLed, grid::NUMBER_OF_LEDS> led_;
};

}
