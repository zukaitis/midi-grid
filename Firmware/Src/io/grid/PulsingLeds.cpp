#include "io/grid/PulsingLeds.h"

#include "io/grid/LedOutput.h"
#include "ThreadConfigurations.h"

#include <freertos/ticks.hpp>
#include <iterator>

namespace grid
{

static const uint32_t PULSE_STEP_INTERVAL = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t PULSE_STEP_COUNT = 15;

PulsingLeds::PulsingLeds( LedOutput& ledOutput ):
        Thread( "PulsingLeds", kPulsingLeds.stackDepth, kPulsingLeds.priority ),
        ledOutput_( ledOutput ),
        led_( 0 )
{
    Thread::Start();
    Thread::Suspend();
}

void PulsingLeds::Run()
{
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( PULSE_STEP_INTERVAL );
    static uint8_t stepNumber = 0;

    while (true)
    {
        Thread::DelayUntil( delayPeriod );

        stepNumber = (stepNumber + 1) % PULSE_STEP_COUNT;

        for (const auto& l : led_)
        {
            Color dimmedColor = l.color;
            if (stepNumber <= 3)
            {
                // y = x / 4
                dimmedColor = dimmedColor * (static_cast<float>(stepNumber + 1) / 4);
            }
            else
            {
                // y = -x / 16
                dimmedColor = dimmedColor * (static_cast<float>(19 - stepNumber) / 16);
            }
            ledOutput_.set( l.coordinates, dimmedColor );
        }
    }
}

void PulsingLeds::add( const Coordinates& coordinates, const Color& color )
{
    led_.push_back( {coordinates, color} ); // TODO: use emplace

    if (1 == led_.size())
    {
        Thread::Resume();
    }
    // don't change output value, it will be set on next flash period
}

void PulsingLeds::remove( const Coordinates& coordinates )
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

void PulsingLeds::removeAll()
{
    led_.clear();
}

}
