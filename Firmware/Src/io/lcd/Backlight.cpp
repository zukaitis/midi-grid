#include "io/lcd/Backlight.h"
#include "hardware/lcd/BacklightDriverInterface.h"

#include "system/gpio_definitions.h"
#include "stm32f4xx_hal.h"

#include "ThreadConfigurations.h"
#include <freertos/ticks.hpp>

#include <cstring>

namespace lcd
{

Backlight::Backlight( hardware::lcd::BacklightDriverInterface& driver ):
    Thread( "Backlight", kBacklight.stackDepth, kBacklight.priority ),
    driver_( driver ),
    appointedIntensity_( 0 ),
    currentIntensity_( 0 )
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

void Backlight::setIntensity( const uint8_t intensity )
{
    uint8_t intensityValue = intensity;
    if (intensityValue > driver_.getMaximumIntensity())
    {
        intensityValue = driver_.getMaximumIntensity();
    }

    appointedIntensity_ = intensityValue;
    appointedIntensityChanged_.Give();
}

uint8_t Backlight::getMaximumIntensity() const
{
    return driver_.getMaximumIntensity();
}

} // namespace lcd
