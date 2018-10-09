#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "Types.h"

namespace mcu {
class GlobalInterrupts;
class Time;
}

namespace grid
{

class GridDriver;

enum LedLightingType
{
    LedLightingType_LIGHT = 0,
    LedLightingType_FLASH,
    LedLightingType_PULSE
};

static const uint8_t kLedFlashingNumberOfColors = 2;

struct FlashingLed
{
    uint8_t positionX;
    uint8_t positionY;
    Color color[kLedFlashingNumberOfColors];
};

struct PulsingLed
{
    uint8_t positionX;
    uint8_t positionY;
};

struct Led
{
    Color color;
    LedLightingType lightingType; // light?flash?pulse
};

class Grid
{
public:
    Grid( GridDriver& gridControl, mcu::GlobalInterrupts& globalInterrupts, mcu::Time& time );
    ~Grid();

    bool areColorsEqual( const Color& color1, const Color& color2 ) const;

    void discardAllPendingButtonEvents();
    bool getButtonEvent( uint8_t& buttonPositionX, uint8_t& buttonPositionY, ButtonEvent& buttonEvent );
    Color getLedColor( const uint8_t ledPositionX, const uint8_t ledPositionY ) const;
    Color getRandomColor();
    void refreshLeds() const;

    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color );
    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color, const LedLightingType lightingType );

    void turnAllLedsOff();

    static const uint8_t numberOfRows = 8;
    static const uint8_t numberOfColumns = 10;
    static const uint8_t numberOfLeds = numberOfRows * numberOfColumns;

private:
    void setLedOutput( uint8_t ledPositionX, uint8_t ledPositionY, const Color color ) const;
    void updateButtonColumnInput();

    GridDriver& gridDriver_;
    mcu::GlobalInterrupts& globalInterrupts_;
    mcu::Time& time_;

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
