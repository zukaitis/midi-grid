#include "io/grid/FlashingLeds.h"

#include "io/grid/LedOutput.h"
#include "ThreadConfigurations.h"

#include "ticks.hpp"
#include <iterator>

namespace grid
{

FlashingLeds::FlashingLeds( LedOutput& ledOutput ):
        Thread( "FlashingLeds", kFlashingLeds.stackDepth, kFlashingLeds.priority ),
        ledOutput_( ledOutput ),
        led_( 0 )
{
    Start();
    Suspend();
}

void FlashingLeds::Run()
{
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( FLASHING_PERIOD );
    static uint8_t flashColorIndex = 0;

    while (true)
    {
        DelayUntil( delayPeriod );

        for (const auto& l : led_)
        {
            ledOutput_.set( l.coordinates, l.color[flashColorIndex] );
        }

        flashColorIndex = (flashColorIndex + 1) % NUMBER_OF_FLASHING_COLORS;
    }
}

void FlashingLeds::add( const Coordinates& coordinates, const FlashingColors& color )
{
    led_.push_back( {coordinates, color} );

    if (1 == led_.size())
    {
        Resume();
    }
    // don't change output value, it will be set on next flash period
}

void FlashingLeds::remove( const Coordinates& coordinates )
{
    // TODO: try some fancy implementation when everything works

    for (auto& l : led_)
    {
        if (coordinates == l.coordinates)
        {
            led_.erase( &l );

            if (led_.empty())
            {
                Suspend();
            }
            break;
        }
    }
}

void FlashingLeds::removeAll()
{
    led_.clear();
}

}
