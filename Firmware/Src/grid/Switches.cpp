#include "grid/Switches.h"
#include "grid/GridDriver.h"
#include "ticks.hpp"

#include <stdlib.h>

namespace grid
{

static const uint8_t kNumberOfRotaryEncoders = 2;
static const uint8_t kNumberOfRotaryEncoderTimeSteps = 10;
static const int8_t kNumberOfRotaryEncoderMicrostepsInStep = 4;

static const int8_t kEncoderState[16] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0 };

Switches::Switches( GridDriver& gridDriver ) :
        gridDriver_( gridDriver )
{
    // active low
    registeredButtonInput_[0] = true;
    registeredButtonInput_[1] = true;
}

Switches::~Switches()
{
}

void Switches::discardAllPendingEvents()
{
    uint8_t unusedUnsignedChar;
    int8_t unusedSignedChar;
    ButtonEvent unusedEvent;

    // call methods while they have pending events
    while (getButtonEvent( unusedUnsignedChar, unusedEvent ))
    {
    }

    while (getRotaryEncoderEvent( unusedUnsignedChar, unusedSignedChar ))
    {
    }
}

// call this method after checking buttons, because it resets flag in GridControl
bool Switches::getRotaryEncoderEvent( uint8_t& rotaryEncoderNumber, int8_t& steps )
{
    static bool encoderChangeDetected = false;

    if (gridDriver_.isSwitchInputUpdated() || encoderChangeDetected)
    {
        encoderChangeDetected = false;
        gridDriver_.resetSwitchInputUpdatedFlag();
        for (uint8_t encoder = 0; encoder < kNumberOfRotaryEncoders; encoder++)
        {
            static int8_t microstep[2] = {0, 0};
            static uint8_t previousEncoderValue[2] = {0, 0};

            for (uint8_t timeStep = 0; timeStep < kNumberOfRotaryEncoderTimeSteps; timeStep++)
            {
                previousEncoderValue[encoder] <<= 2;
                previousEncoderValue[encoder] |= gridDriver_.getRotaryEncodersInput( encoder, timeStep );
                previousEncoderValue[encoder] &= 0x0F;
                microstep[encoder] += kEncoderState[previousEncoderValue[encoder]];
            }

            // only respond every 4 microsteps (1 physical step)
            if (abs( microstep[encoder] ) >= kNumberOfRotaryEncoderMicrostepsInStep )
            {
                static uint32_t previousEventTime[2] = {0, 0};
                int8_t velocityMultiplier;
                
                const uint32_t interval = freertos::Ticks::TicksToMs( freertos::Ticks::GetTicks() ) - previousEventTime[encoder];

                if (interval > 500)
                {
                    velocityMultiplier = 1;
                }
                else if (interval > 200)
                {
                    velocityMultiplier = 2;
                }
                else if (interval > 50)
                {
                    velocityMultiplier = 4;
                }
                else
                {
                    velocityMultiplier = 8;
                }

                previousEventTime[encoder] = freertos::Ticks::TicksToMs( freertos::Ticks::GetTicks() );
                rotaryEncoderNumber = encoder;
                steps = ((microstep[encoder] > 0) ? 1 * velocityMultiplier : -1 * velocityMultiplier);
                microstep[encoder] %= 4;
                return true;
            }
        }
    }
    return false;
}

bool Switches::getButtonEvent( uint8_t& buttonNumber, ButtonEvent& buttonEvent )
{
    static bool buttonChangeDetected = false;

    if (gridDriver_.isSwitchInputUpdated() || buttonChangeDetected)
    {
        buttonChangeDetected = false; // reset this variable every time, it will be set back if necessary
        for (uint8_t button = 0; button < numberOfButtons_; button++)
        {
            if (gridDriver_.isButtonInputStable(button))
            {
                const bool buttonInput = gridDriver_.getButtonInput(button);
                if (registeredButtonInput_[button] != buttonInput)
                {
                    buttonEvent = buttonInput ? ButtonEvent_RELEASED : ButtonEvent_PRESSED; // active low
                    registeredButtonInput_[button] = buttonInput;
                    buttonNumber = button;
                    buttonChangeDetected = true;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Switches::isButtonPressed( const uint8_t buttonNumber )
{
    bool isPressed = false;
    if (buttonNumber < numberOfButtons_)
    {
        isPressed = !registeredButtonInput_[buttonNumber];
    }
    return isPressed;
}

} // namespace grid
