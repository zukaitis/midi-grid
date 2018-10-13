#ifndef GRID_SWITCHES_H_
#define GRID_SWITCHES_H_

#include "Types.h"

namespace mcu {
class Time;
}

namespace grid
{

class GridDriver;

// class used to acquire values from two additional buttons and rotary encoders
class Switches
{
public:
    Switches( GridDriver& gridControl, mcu::Time& time );
    ~Switches();

    void discardAllPendingEvents();

    bool getButtonEvent( uint8_t& buttonNumber, ButtonEvent& buttonEvent );
    bool getRotaryEncoderEvent( uint8_t& rotaryEncoderNumber, int8_t& steps );

    bool isButtonPressed( const uint8_t buttonNumber );

    enum Buttons
    {
        additionalNoteButton = 0U,
        internalMenuButton = 1U,
    };

private:
    GridDriver& gridDriver_;
    mcu::Time& time_;

    static const uint8_t numberOfButtons_ = 2;

    bool registeredButtonInput_[numberOfButtons_];
};

} // namespace grid

#endif // GRID_SWITCHES_H_
