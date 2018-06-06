#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "grid/GridControl.h"
#include "stm32f4xx_hal.h"
#include "Types.h"

namespace grid_control
{
    class GridControl;
}

namespace grid
{

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
    Colour colour[2];
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

static const uint8_t NUMBER_OF_ROWS = 8;
static const uint8_t NUMBER_OF_COLUMNS = 10;

static const uint32_t LED_FLASH_PERIOD_MS = 250; // 120bpm - default flashing rate
static const uint32_t LED_PULSE_STEP_PERIOD_MS = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t LED_PULSE_STEP_COUNT = 15;

class Grid
{
public:
    Grid();
    ~Grid();

    bool areColoursEqual( const Colour& colour1, const Colour& colour2 ) const;

    void discardAllPendingButtonEvents();
    void enable();
    bool getButtonEvent( uint8_t* buttonPositionX, uint8_t* buttonPositionY, ButtonEvent* buttonEvent );
    Colour getLedColour( uint8_t ledPositionX, uint8_t ledPositionY ) const;
    void initialize();
    void refreshLeds() const;

    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour );
    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType );

    void turnAllLedsOff();

private:
    void setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const;

    grid_control::GridControl& gridControl;
    bool initialized_ = false;

    Led led_[10][8];

    uint16_t registeredButtonInput_[grid_control::NUMBER_OF_COLUMNS];

    FlashingLed flashingLed_[64];
    uint8_t numberOfFlashingLeds_ = 0;

    PulsingLed pulsingLed_[64];
    uint8_t numberOfPulsingLeds_ = 0;
};

} // namespace
#endif // GRID_BUTTONS_H_
