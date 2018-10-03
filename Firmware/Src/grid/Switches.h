#ifndef GRID_SWITCHES_H_
#define GRID_SWITCHES_H_

#include "grid/GridControl.h"
#include "Types.h"

class Time;

namespace grid
{
namespace switches
{

enum Buttons
{
    kUndefinedButton = 0U,
    kInternalMenuButton = 1U,
};

static const uint8_t kNumberOfButtons = 2;
static const uint8_t kNumberOfRotaryEncoders = 2;
static const uint8_t kNumberOfRotaryEncoderTimeSteps = 10;
static const int8_t kNumberOfRotaryEncoderMicrostepsInStep = 4;

static const int8_t kEncoderState[16] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0 };

// class used to acquire values from two additional buttons and rotary encoders
class Switches
{
public:
    Switches( grid_control::GridControl& gridControl, Time& time );
    ~Switches();

    void discardAllPendingEvents();

    bool getButtonEvent( uint8_t& buttonNumber, ButtonEvent& buttonEvent );
    bool getRotaryEncoderEvent( uint8_t& rotaryEncoderNumber, int8_t& steps );

    bool isButtonPressed( const uint8_t buttonNumber );

private:
    grid_control::GridControl& gridControl_;
    Time& time_;

    bool registeredButtonInput_[kNumberOfButtons];
};

} // namespace switches
} // namespace grid

#endif // GRID_SWITCHES_H_
