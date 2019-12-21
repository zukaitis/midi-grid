#pragma once

#include "io/HardwareInputInterface.h"
#include "types/ButtonAction.h"
#include "types/Color.h"
#include "types/Coordinates.h"

namespace grid
{

static const uint8_t numberOfColumns = 10;
static const uint8_t numberOfRows = 8;
static const uint8_t NUMBER_OF_LEDS = numberOfColumns * numberOfRows;

namespace button
{
    static const Coordinates ARROW_UP = {9, 4};
    static const Coordinates ARROW_DOWN = {9, 7};
    static const Coordinates ARROW_LEFT = {9, 5};
    static const Coordinates ARROW_RIGHT = {9, 6};
}

struct ButtonEvent
{
    ButtonAction action;
    Coordinates coordinates;
};

enum class LedLightingType : uint8_t
{
    LIGHT = 0,
    FLASH,
    PULSE
};

class GridInterface : public HardwareInputInterface<ButtonEvent>
{
public:
    virtual ~GridInterface() {};

    virtual bool waitForInput( ButtonEvent& event ) override = 0;
    virtual void discardPendingInput() override = 0;

    virtual Color getLedColor( const Coordinates& coordinates ) const = 0;

    virtual void setLed( const Coordinates& coordinates, const Color& color, const LedLightingType lightingType = LedLightingType::LIGHT ) = 0;

    virtual void turnAllLedsOff() = 0;
};

}
