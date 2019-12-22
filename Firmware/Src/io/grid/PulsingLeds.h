#pragma once

#include "io/grid/PulsingLedsInterface.h"
#include "io/grid/GridInterface.h"

#include "types/Color.h"
#include "types/Coordinates.h"

#include <freertos/thread.hpp>
#include <etl/array.h>
#include <etl/vector.h>

namespace grid
{

class LedOutputInterface;

class PulsingLeds: public PulsingLedsInterface, private freertos::Thread
{
public:
    PulsingLeds( LedOutputInterface& ledOutput );

    void add( const Coordinates& coordinates, const Color& color ) override;
    void remove( const Coordinates& coordinates ) override;
    void removeAll() override;

private:
    void Run() override;
    void setOutputValues();

    struct PulsingLed
    {
        Coordinates coordinates;
        Color color;
    };

    LedOutputInterface& ledOutput_;
    etl::vector<PulsingLed, grid::NUMBER_OF_LEDS> led_;
};

}
