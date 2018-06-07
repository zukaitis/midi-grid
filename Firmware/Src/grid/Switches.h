#ifndef GRID_SWITCHES_H_
#define GRID_SWITCHES_H_

#include "grid/GridControl.h"
#include "Types.h"

namespace switches
{

static const uint8_t NUMBER_OF_BUTTONS = 2;
static const uint8_t NUMBER_OF_ROTARY_ENCODERS = 2;
static const uint8_t NUMBER_OF_ROTARY_ENCODER_TIME_STEPS = 10;

// class used to acquire values from two additional buttons and rotary encoders
class Switches
{
public:
    Switches();
    ~Switches();

    void discardAllPendingEvents();

    bool getButtonEvent( uint8_t& buttonNumber, ButtonEvent& buttonEvent );
    bool getRotaryEncoderEvent( uint8_t& rotaryEncoderNumber, int8_t& steps );

    bool isButtonPressed( uint8_t buttonNumber );

private:
    grid_control::GridControl& gridControl;

    bool registeredButtonInput_[NUMBER_OF_BUTTONS] = {true, true}; // active low
};

} // namespace

#endif // GRID_SWITCHES_H_
