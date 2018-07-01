#ifndef GRID_SWITCHES_H_
#define GRID_SWITCHES_H_

#include "grid/GridControl.h"
#include "Types.h"

namespace grid
{
namespace switches
{

static const uint8_t NUMBER_OF_BUTTONS = 2;
static const uint8_t NUMBER_OF_ROTARY_ENCODERS = 2;
static const uint8_t NUMBER_OF_ROTARY_ENCODER_TIME_STEPS = 10;
static const int8_t NUMBER_OF_MICROSTEPS_IN_STEP = 4;

static const int8_t ENCODER_STATES[16] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0 };

// class used to acquire values from two additional buttons and rotary encoders
class Switches
{
public:
    Switches();
    ~Switches();

    void discardAllPendingEvents();

    bool getButtonEvent( uint8_t& buttonNumber, ButtonEvent& buttonEvent );
    bool getRotaryEncoderEvent( uint8_t& rotaryEncoderNumber, int8_t& steps );

    bool isButtonPressed( const uint8_t buttonNumber );

private:
    grid::grid_control::GridControl& gridControl;

    bool registeredButtonInput_[NUMBER_OF_BUTTONS];
};

} // namespace switches
} // namespace grid

#endif // GRID_SWITCHES_H_
