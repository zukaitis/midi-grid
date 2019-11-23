#include "io/grid/Grid.h"

#include "system/GlobalInterrupts.hpp"
#include "ThreadConfigurations.h"

#include "freertos/ticks.hpp"
#include <cmath>

namespace grid
{

Coordinates GRID_LIMITS = { numberOfColumns, numberOfRows };

Grid::Grid( hardware::grid::InputInterface& gridDriverInput, hardware::grid::OutputInterface& gridDriverOutput, mcu::GlobalInterrupts& globalInterrupts ) :
        buttonInput_( ButtonInput( gridDriverInput, globalInterrupts ) ),
        ledOutput_( LedOutput( gridDriverOutput ) ),
        flashingLeds_( FlashingLeds( ledOutput_ ) ),
        pulsingLeds_( PulsingLeds( ledOutput_ ) )
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
    return led_[coordinates.x][coordinates.y].color;
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
            case LedLightingType::LIGHT:
                ledOutput_.set( coordinates, color );
                break;
            case LedLightingType::FLASH:
                flashingLeds_.add( coordinates, { led_[coordinates.x][coordinates.y].color, color } );
                break;
            case LedLightingType::PULSE:
                pulsingLeds_.add( coordinates, color );
                break;
            default:
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

    // todo: also remove flashing, pulsing leds and reset colors to zeros
}


}// namespace

