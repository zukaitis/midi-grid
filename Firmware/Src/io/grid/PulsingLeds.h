#pragma once

#include "io/grid/GridInterface.h"

#include "types/Color.h"
#include "types/Coordinates.h"

#include <freertos/thread.hpp>
#include <etl/array.h>
#include <etl/vector.h>

namespace grid
{

class LedOutputInterface;

class PulsingLeds: private freertos::Thread
{
public:
    PulsingLeds( LedOutputInterface& ledOutput );
    virtual ~PulsingLeds() {};

    void add( const Coordinates& coordinates, const Color& color );
    void remove( const Coordinates& coordinates );
    void removeAll();

private:
    void Run() override;

    struct PulsingLed
    {
        Coordinates coordinates;
        Color color;
    };

    LedOutputInterface& ledOutput_;
    etl::vector<PulsingLed, grid::NUMBER_OF_LEDS> led_;
};

}
