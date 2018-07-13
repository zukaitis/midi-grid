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

static const uint8_t NUMBER_OF_ROWS = 8;
static const uint8_t NUMBER_OF_COLUMNS = 10;
static const uint8_t NUMBER_OF_PAD_COLUMNS = 8;
static const uint8_t NUMBER_OF_LEDS = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS;

static const uint32_t LED_FLASH_PERIOD_MS = 250; // 120bpm - default flashing rate
static const uint8_t LED_FLASHING_NUMBER_OF_COLOURS = 2;
static const uint32_t LED_PULSE_STEP_PERIOD_MS = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t LED_PULSE_STEP_COUNT = 15;

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
    Colour colour[LED_FLASHING_NUMBER_OF_COLOURS];
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
    Grid( grid_control::GridControl& gridControl );
    ~Grid();

    bool areColoursEqual( const Colour& colour1, const Colour& colour2 ) const;

    void discardAllPendingButtonEvents();
    void enable();
    bool getButtonEvent( uint8_t& buttonPositionX, uint8_t& buttonPositionY, ButtonEvent& buttonEvent );
    Colour getLedColour( const uint8_t ledPositionX, const uint8_t ledPositionY ) const;
    void initialize();
    void refreshLeds() const;

    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour );
    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType );

    void turnAllLedsOff();

private:
    void setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const;

    grid_control::GridControl& gridControl_;
    bool initialized_;

    Led led_[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];

    uint16_t registeredButtonInput_[grid_control::NUMBER_OF_COLUMNS];

    FlashingLed flashingLed_[NUMBER_OF_LEDS];
    uint8_t numberOfFlashingLeds_;

    PulsingLed pulsingLed_[NUMBER_OF_LEDS];
    uint8_t numberOfPulsingLeds_;
};

} // namespace
#endif // GRID_BUTTONS_H_
