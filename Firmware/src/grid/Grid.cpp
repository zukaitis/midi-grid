#include "grid/Grid.h"

#include "grid/ButtonInputInterface.h"
#include "grid/GridInterface.h"
#include "grid/LedOutputInterface.h"
#include "grid/FlashingLedsInterface.h"
#include "grid/PulsingLedsInterface.h"

namespace grid
{

Grid::Grid( ButtonInputInterface* buttonInput,
    LedOutputInterface* ledOutput,
    FlashingLedsInterface* flashingLeds,
    PulsingLedsInterface* pulsingLeds ) :
        buttonInput_( *buttonInput ),
        ledOutput_( *ledOutput ),
        flashingLeds_( *flashingLeds ),
        pulsingLeds_( *pulsingLeds ),
        led_()
{
}

bool Grid::waitForInput( ButtonEvent* event )
{
    return buttonInput_.waitForEvent( event );
}

void Grid::discardPendingInput()
{
    buttonInput_.discardPendingEvents();
}

Color Grid::getLedColor( const Coordinates& coordinates ) const
{
    Color color(0, 0, 0);
    if (coordinates < gridLimits)
    {
        color = led_.at(coordinates.x).at(coordinates.y).color;
    }
    return color;
}

void Grid::setLed( const Coordinates& coordinates, const Color& color, const LedLightingType lightingType )
{
    if (coordinates < gridLimits)
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

void Grid::setLed( const Coordinates& coordinates, const Color& color )
{
    setLed( coordinates, color, LedLightingType::LIGHT );
}

void Grid::turnAllLedsOff()
{
    ledOutput_.setAllOff();
    flashingLeds_.removeAll();
    pulsingLeds_.removeAll();

    led_ = {};
}

} // namespace

