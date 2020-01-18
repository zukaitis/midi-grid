#include "io/grid/Grid.h"

#include "io/grid/ButtonInputInterface.h"
#include "io/grid/LedOutputInterface.h"
#include "io/grid/FlashingLedsInterface.h"
#include "io/grid/PulsingLedsInterface.h"

namespace grid
{

Coordinates GRID_LIMITS = { numberOfColumns, numberOfRows };

Grid::Grid( ButtonInputInterface& buttonInput,
    LedOutputInterface& ledOutput,
    FlashingLedsInterface& flashingLeds,
    PulsingLedsInterface& pulsingLeds ) :
        buttonInput_( buttonInput ),
        ledOutput_( ledOutput ),
        flashingLeds_( flashingLeds ),
        pulsingLeds_( pulsingLeds ),
        led_()
{
}

bool Grid::waitForInput( ButtonEvent& event )
{
    return buttonInput_.waitForEvent( event );
}

void Grid::discardPendingInput()
{
    buttonInput_.discardPendingEvents();
}

Color Grid::getLedColor( const Coordinates& coordinates ) const
{
    return led_.at(coordinates.x).at(coordinates.y).color;
}

void Grid::setLed( const Coordinates& coordinates, const Color& color, const LedLightingType lightingType )
{
    if (coordinates < GRID_LIMITS)
    {
        // remove led from flashing or pulsing list if it's in that list and proceed with setting the led
        if (LedLightingType::FLASH == led_[coordinates.x][coordinates.y].lightingType)
        {
            flashingLeds_.remove( coordinates );
        }
        else if (LedLightingType::PULSE == led_[coordinates.x][coordinates.y].lightingType)
        {
            pulsingLeds_.remove( coordinates );
        }

        switch (lightingType)
        {
            case LedLightingType::FLASH:
                flashingLeds_.add( coordinates, { led_[coordinates.x][coordinates.y].color, color } );
                break;
            case LedLightingType::PULSE:
                pulsingLeds_.add( coordinates, color );
                break;
            case LedLightingType::LIGHT:
            default:
                ledOutput_.set( coordinates, color );
                break;
        }

        led_[coordinates.x][coordinates.y].lightingType = lightingType;
        led_[coordinates.x][coordinates.y].color = color;
    }
}

void Grid::turnAllLedsOff()
{
    ledOutput_.setAllOff();
    flashingLeds_.removeAll();
    pulsingLeds_.removeAll();

    led_ = {};
}

} // namespace

