#pragma once

#include "io/HardwareInputInterface.h" // TODO: replace this nonsense name
#include "types/ButtonAction.h"
#include "types/Color.h"
#include "types/Coordinates.h"

namespace grid
{

const uint8_t numberOfColumns = 10;
const uint8_t numberOfRows = 8;
const Coordinates gridLimits = { numberOfColumns, numberOfRows };
const uint8_t NUMBER_OF_LEDS = numberOfColumns * numberOfRows;

namespace button
{
    const Coordinates ARROW_UP = {9, 4};
    const Coordinates ARROW_DOWN = {9, 7};
    const Coordinates ARROW_LEFT = {9, 5};
    const Coordinates ARROW_RIGHT = {9, 6};
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
    virtual ~GridInterface() = default;

    bool waitForInput( ButtonEvent* event ) override = 0;
    void discardPendingInput() override = 0;

    virtual Color getLedColor( const Coordinates& coordinates ) const = 0;

    virtual void setLed( const Coordinates& coordinates, const Color& color, LedLightingType lightingType ) = 0;
    virtual void setLed( const Coordinates& coordinates, const Color& color ) = 0;

    virtual void turnAllLedsOff() = 0;
};

}  // namespace grid
