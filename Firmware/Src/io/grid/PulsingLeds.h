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

class PulsingLeds: private freertos::Thread
{
public:
    PulsingLeds( LedOutput& ledOutput );

    void add( const Coordinates& coordinates, const Color& color );
    void remove( const Coordinates& coordinates );
    void removeAll();

private:
    virtual void Run();

    struct PulsingLed
    {
        Coordinates coordinates;
        Color color;
    };

    LedOutput& ledOutput_;
    etl::vector<PulsingLed, grid::NUMBER_OF_LEDS> led_;
};

}
