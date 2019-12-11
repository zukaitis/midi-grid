#include "io/grid/FlashingLeds.h"

#include "io/grid/LedOutputInterface.h"
#include "ThreadConfigurations.h"

#include <freertos/ticks.hpp>
#include <iterator>

namespace grid
{

static const uint32_t FLASHING_PERIOD = 250; // 120bpm - default flashing rate

FlashingLeds::FlashingLeds( LedOutputInterface& ledOutput ):
        Thread( "FlashingLeds", kFlashingLeds.stackDepth, kFlashingLeds.priority ),
        ledOutput_( ledOutput ),
        led_( 0 )
{
    Thread::Start();
    Thread::Suspend();
}

void FlashingLeds::Run()
{
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( FLASHING_PERIOD );
    static uint8_t flashColorIndex = 0;

    while (true)
    {
        Thread::DelayUntil( delayPeriod );

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
        Thread::Resume();
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
                Thread::Suspend();
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
