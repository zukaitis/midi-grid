#include "lcd/backlight/Backlight.h"
#include "hardware/lcd/BacklightDriverInterface.h"

#include "ThreadConfigurations.h"
#include <freertos/ticks.hpp>

#include <cstring>

namespace lcd
{

Backlight::Backlight( hardware::lcd::BacklightDriverInterface* driver ):
    Thread( "Backlight", kBacklight.stackDepth, kBacklight.priority ),
    driver_( *driver ),
    appointedIntensity_( 0 ),
    currentIntensity_( 0 ),
    appointedIntensityChanged_( false )
{
}

void Backlight::Run()
{
    static const TickType_t updatePeriod = freertos::Ticks::MsToTicks( 50 );

    if (currentIntensity_ != appointedIntensity_)
    {
        if (currentIntensity_ > appointedIntensity_)
        {
            driver_.setIntensity( --currentIntensity_ );
        }
        else
        {
            driver_.setIntensity( ++currentIntensity_ );
        }

        DelayUntil( updatePeriod );
    }
    else
    {
        // block until intensity is changed
        appointedIntensityChanged_.Take();
    }
}

void Backlight::initialize()
{
    driver_.initialize();
    Thread::Start();
}

void Backlight::setIntensity( uint8_t intensity )
{
    if (intensity > maxIntensity())
    {
        intensity = maxIntensity();
    }

    appointedIntensity_ = intensity;
    appointedIntensityChanged_.Give();
}

} // namespace lcd
