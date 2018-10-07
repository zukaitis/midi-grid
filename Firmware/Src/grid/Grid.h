#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "Types.h"

class GlobalInterrupts;
class Time;

namespace grid
{

class GridControl;

enum LedLightingType
{
    LedLightingType_LIGHT = 0,
    LedLightingType_FLASH,
    LedLightingType_PULSE
};

static const uint8_t kLedFlashingNumberOfColours = 2;

struct FlashingLed
{
    uint8_t positionX;
    uint8_t positionY;
    Colour colour[kLedFlashingNumberOfColours];
};

struct PulsingLed
{
    uint8_t positionX;
    uint8_t positionY;
};

struct Led
{
    Colour colour;
    LedLightingType lightingType; // light?flash?pulse
};

class Grid
{
public:
    Grid( GridControl& gridControl, GlobalInterrupts& globalInterrupts, Time& time );
    ~Grid();

    bool areColoursEqual( const Colour& colour1, const Colour& colour2 ) const;

    void discardAllPendingButtonEvents();
    bool getButtonEvent( uint8_t& buttonPositionX, uint8_t& buttonPositionY, ButtonEvent& buttonEvent );
    Colour getLedColour( const uint8_t ledPositionX, const uint8_t ledPositionY ) const;
    Colour getRandomColour();
    void refreshLeds() const;

    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour );
    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType );

    void turnAllLedsOff();

    static const uint8_t numberOfRows = 8;
    static const uint8_t numberOfColumns = 10;
    static const uint8_t numberOfLeds = numberOfRows * numberOfColumns;

private:
    void setLedOutput( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const;
    void updateButtonColumnInput();

    GridControl& gridControl_;
    GlobalInterrupts& globalInterrupts_;
    Time& time_;

    Led led_[numberOfColumns][numberOfRows];

    uint8_t buttonColumnInput_[numberOfColumns];
    uint8_t registeredButtonColumnInput_[numberOfColumns];

    FlashingLed flashingLed_[numberOfLeds];
    uint8_t numberOfFlashingLeds_;

    PulsingLed pulsingLed_[numberOfLeds];
    uint8_t numberOfPulsingLeds_;
};

} // namespace
#endif // GRID_BUTTONS_H_
