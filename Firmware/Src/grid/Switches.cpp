#include "grid/Switches.h"

namespace grid
{
namespace switches
{

Switches::Switches() :
        gridControl( grid_control::GridControl::getInstance() )
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


    if (gridControl.switchInputUpdated || encoderChangeDetected)
    {
        encoderChangeDetected = false;
        gridControl.switchInputUpdated = false;
        for (uint8_t encoder = 0; encoder < NUMBER_OF_ROTARY_ENCODERS; encoder++)
        {
            static int8_t microstep[2] = {0, 0};
            static uint8_t previousEncoderValue[2] = {0, 0};

            for (uint8_t timeStep = 0; timeStep < NUMBER_OF_ROTARY_ENCODER_TIME_STEPS; timeStep++)
            {
                previousEncoderValue[encoder] <<= 2;
                previousEncoderValue[encoder] |= gridControl.getRotaryEncodersInput( encoder, timeStep );
                previousEncoderValue[encoder] &= 0x0F;
                microstep[encoder] += ENCODER_STATES[previousEncoderValue[encoder]];
            }

            if ((microstep[encoder] >= NUMBER_OF_MICROSTEPS_IN_STEP) || (microstep[encoder] <= -NUMBER_OF_MICROSTEPS_IN_STEP))
            {
                static uint32_t previousEventTime[2] = {0, 0};
                int8_t velocityMultiplier;
                // only respond every 4 microsteps (1 physical step)
                const uint32_t interval = HAL_GetTick() - previousEventTime[encoder];

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

                previousEventTime[encoder] = HAL_GetTick();
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

    if (gridControl.switchInputUpdated || buttonChangeDetected)
    {
        buttonChangeDetected = false; //reset this variable every time, it will be set back if necessary
        for (uint8_t button = 0; button < NUMBER_OF_BUTTONS; button++)
        {
            if (gridControl.isButtonInputStable(button))
            {
                const bool buttonInput = gridControl.getButtonInput(button);
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
    if (buttonNumber < NUMBER_OF_BUTTONS)
    {
        isPressed = !registeredButtonInput_[buttonNumber];
    }
    return isPressed;
}

} // namespace switches
} // namespace grid
