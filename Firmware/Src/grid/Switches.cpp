#include "grid/Switches.h"
#include "system/Time.h"

namespace grid
{
namespace switches
{

Switches::Switches( grid_control::GridControl& gridControl, Time& time ) :
        gridControl_( gridControl ),
        time_( time )
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


    if (gridControl_.isSwitchInputUpdated() || encoderChangeDetected)
    {
        encoderChangeDetected = false;
        gridControl_.resetSwitchInputUpdatedFlag();
        for (uint8_t encoder = 0; encoder < kNumberOfRotaryEncoders; encoder++)
        {
            static int8_t microstep[2] = {0, 0};
            static uint8_t previousEncoderValue[2] = {0, 0};

            for (uint8_t timeStep = 0; timeStep < kNumberOfRotaryEncoderTimeSteps; timeStep++)
            {
                previousEncoderValue[encoder] <<= 2;
                previousEncoderValue[encoder] |= gridControl_.getRotaryEncodersInput( encoder, timeStep );
                previousEncoderValue[encoder] &= 0x0F;
                microstep[encoder] += kEncoderState[previousEncoderValue[encoder]];
            }

            if ((microstep[encoder] >= kNumberOfRotaryEncoderMicrostepsInStep) || (microstep[encoder] <= -kNumberOfRotaryEncoderMicrostepsInStep))
            {
                static uint32_t previousEventTime[2] = {0, 0};
                int8_t velocityMultiplier;
                // only respond every 4 microsteps (1 physical step)
                const uint32_t interval = time_.getSystemTick() - previousEventTime[encoder];

                if (interval >= 500)
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

                previousEventTime[encoder] = time_.getSystemTick();
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

    if (gridControl_.isSwitchInputUpdated() || buttonChangeDetected)
    {
        buttonChangeDetected = false; // reset this variable every time, it will be set back if necessary
        for (uint8_t button = 0; button < kNumberOfButtons; button++)
        {
            if (gridControl_.isButtonInputStable(button))
            {
                const bool buttonInput = gridControl_.getButtonInput(button);
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
    if (buttonNumber < kNumberOfButtons)
    {
        isPressed = !registeredButtonInput_[buttonNumber];
    }
    return isPressed;
}

} // namespace switches
} // namespace grid
