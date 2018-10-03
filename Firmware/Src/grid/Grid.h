#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "grid/GridControl.h"
#include "Types.h"

namespace grid_control
{
    class GridControl;
}

class Time;

namespace grid
{

static const uint8_t kNumberOfRows = 8;
static const uint8_t kNumberOfColumns = 10;
static const uint8_t kNumberOfPadColumns = 8;
static const uint8_t kNumberOfLeds = kNumberOfRows * kNumberOfColumns;

static const uint32_t kLedFlashingPeriod = 250; // 120bpm - default flashing rate
static const uint8_t kLedFlashingNumberOfColours = 2;
static const uint32_t kLedPulseStepInterval = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t kLedPulseStepCount = 15;

enum LedLightingType
{
    LedLightingType_LIGHT = 0,
    LedLightingType_FLASH,
    LedLightingType_PULSE
};

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
    Grid( grid_control::GridControl& gridControl, Time& time );
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

private:
    void setLedOutput( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const;

    grid_control::GridControl& gridControl_;
    Time& time_;

    Led led_[kNumberOfColumns][kNumberOfRows];

    uint16_t registeredButtonInput_[grid_control::kNumberOfVerticalSegments];

    FlashingLed flashingLed_[kNumberOfLeds];
    uint8_t numberOfFlashingLeds_;

    PulsingLed pulsingLed_[kNumberOfLeds];
    uint8_t numberOfPulsingLeds_;
};

} // namespace
#endif // GRID_BUTTONS_H_
