#pragma once

#include "io/grid/FlashingLedsInterface.h"
#include "io/grid/GridInterface.h"

#include "types/Coordinates.h"

#include <freertos/thread.hpp>
#include <etl/vector.h>

namespace grid
{

class LedOutputInterface;

class FlashingLeds: public FlashingLedsInterface, private freertos::Thread
{
public:
    FlashingLeds( LedOutputInterface& ledOutput );

    void add( const Coordinates& coordinates, const FlashingColors& color ) override;
    void remove( const Coordinates& coordinates ) override;
    void removeAll() override;

private:
    virtual void Run();

    void setOutputValues();

    struct FlashingLed
    {
        Coordinates coordinates;
        FlashingColors color;
    };

    LedOutputInterface& ledOutput_;
    etl::vector<FlashingLed, grid::NUMBER_OF_LEDS> led_;
};

}
