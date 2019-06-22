#ifndef GRID_BUTTONS_HPP_
#define GRID_BUTTONS_HPP_

#include "Types.h"

#include "thread.hpp"
#include "queue.hpp"

namespace mcu
{
class GlobalInterrupts;
}

namespace grid
{

class GridDriver;

static const uint8_t numberOfRows = 8;
static const uint8_t numberOfColumns = 10;
static const uint8_t numberOfLeds = numberOfRows * numberOfColumns;

static const uint8_t arrowButtonPositionX = 9;
static const uint8_t upButtonPositionY = 4;
static const uint8_t downButtonPositionY = 7;
static const uint8_t leftButtonPositionY = 5;
static const uint8_t rightButtonPositionY = 6;

enum LedLightingType
{
    LedLightingType_LIGHT = 0,
    LedLightingType_FLASH,
    LedLightingType_PULSE
};

class GridLedOutput
{
public:
    GridLedOutput( GridDriver& gridDriver );
    ~GridLedOutput();

    void set( uint8_t ledPositionX, uint8_t ledPositionY, const Color color ) const;

private:
    GridDriver& gridDriver_;
};

class FlashingLeds: private freertos::Thread
{
public:
    FlashingLeds( GridLedOutput& gridLedOutput );
    ~FlashingLeds();

    void add( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color1, const Color color2 );
    void remove( const uint8_t ledPositionX, const uint8_t ledPositionY );

private:
    virtual void Run();

    struct FlashingLed
    {
        uint8_t positionX;
        uint8_t positionY;
        Color color[2];
    };

    GridLedOutput& ledOutput_;

    FlashingLed led_[numberOfLeds];
    uint8_t numberOfFlashingLeds_;
};

class PulsingLeds: private freertos::Thread
{
public:
    PulsingLeds( GridLedOutput& gridLedOutput );
    ~PulsingLeds();

    void add( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color );
    void remove( const uint8_t ledPositionX, const uint8_t ledPositionY );

private:
    virtual void Run();

    struct PulsingLed
    {
        uint8_t positionX;
        uint8_t positionY;
        Color color;
    };

    GridLedOutput& ledOutput_;

    PulsingLed led_[numberOfLeds];
    uint8_t numberOfPulsingLeds_;
};

struct Led
{
    Color color;
    LedLightingType lightingType;
};

class Grid : private freertos::Thread
{
public:
    Grid( GridDriver& gridDriver, mcu::GlobalInterrupts& globalInterrupts );
    ~Grid();

    struct ButtonEvent
    {
        ButtonAction action;
        uint8_t positionX;
        uint8_t positionY;
    };

    bool areColorsEqual( const Color& color1, const Color& color2 ) const;

    void discardAllPendingButtonEvents();
    bool waitForButtonEvent( ButtonEvent& event );
    Color getLedColor( const uint8_t ledPositionX, const uint8_t ledPositionY ) const;
    Color getRandomColor();

    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color );
    void setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color, const LedLightingType lightingType );

    void turnAllLedsOff();

    inline bool waitForInput( ButtonEvent& event )
    {
        return waitForButtonEvent( event );
    };

    inline void discardPendingInput()
    {
        discardAllPendingButtonEvents();
    };

private:
    void updateButtonColumnInput();

    virtual void Run();

    GridDriver& gridDriver_;
    GridLedOutput ledOutput_;
    FlashingLeds flashingLeds_;
    PulsingLeds pulsingLeds_;
    mcu::GlobalInterrupts& globalInterrupts_;
    freertos::Queue buttonInputEvents_;

    Led led_[numberOfColumns][numberOfRows];
    uint8_t buttonColumnInput_[numberOfColumns];
    uint8_t registeredButtonColumnInput_[numberOfColumns];
};

} // namespace
#endif // GRID_BUTTONS_HPP_
