#include "grid/Switches.h"

namespace switches
{

Switches::Switches() : gridControl( grid_control::GridControl::getInstance() )
{
}

Switches::~Switches()
{
}

// call this method after checking buttons, because it resets flag in GridControl
bool Switches::getRotaryEncoderEvent( uint8_t* rotaryEncoderNumber, int8_t* steps )
{
    static const int8_t encoderStates[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0};
    static bool encoderChangeDetected = false;
    static uint8_t previousEncoderValue[2] = {0, 0};
    int8_t timeStep;
    int8_t outputSteps = 0;

    if (gridControl.switchInputUpdated || encoderChangeDetected)
    {
        encoderChangeDetected = false;
        gridControl.switchInputUpdated = false;
        for (uint8_t encoder = 0; encoder < NUMBER_OF_ROTARY_ENCODERS; encoder++)
        {
            for (timeStep=0; timeStep<10/*NUMBER_OF_ROTARY_ENCODER_STEPS*/; timeStep++)
            {
                previousEncoderValue[encoder] <<= 2;
                previousEncoderValue[encoder] |= gridControl.getRotaryEncodersInput(encoder, timeStep);
                previousEncoderValue[encoder] &= 0x0F;
                outputSteps += encoderStates[previousEncoderValue[encoder]];
            }
            if (0 != outputSteps)
            {
                // only return steps of one encoder, other one will be checked on another cycle
                *rotaryEncoderNumber = encoder;
                *steps = outputSteps;
                //encoderChangeDetected = true;
                return true;
            }
        }
    }
    return false;
}

bool Switches::getButtonEvent( uint8_t* buttonNumber, ButtonEvent* buttonEvent )
{
    static bool buttonChangeDetected = false;
    int8_t button;
    bool buttonInput;

    if (gridControl.switchInputUpdated || buttonChangeDetected)
    {
        buttonChangeDetected = false; //reset this variable every time, it will be set back if necessary
        for (button=0; button<NUMBER_OF_BUTTONS; button++)
        {
            if (gridControl.isButtonInputStable(button))
            {
                buttonInput = gridControl.getButtonInput(button);
                if (registeredButtonInput_[button] != buttonInput)
                {
                    *buttonEvent = buttonInput ? ButtonEvent_RELEASED : ButtonEvent_PRESSED; // active low
                    registeredButtonInput_[button] = buttonInput;
                    *buttonNumber = button;
                    buttonChangeDetected = true;
                    return true;
                }
            }
        }
    }
    return false;
}

}
