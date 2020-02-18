#include "grid/GridContainer.h"

namespace grid
{

GridContainer::GridContainer( 
    hardware::grid::InputInterface* gridDriverInput,
    hardware::grid::OutputInterface* gridDriverOutput,
    mcu::GlobalInterruptsInterface* globalInterrupts ):
        buttonInput_( ButtonInput( gridDriverInput, globalInterrupts ) ),
        ledOutput_( LedOutput( gridDriverOutput ) ),
        flashingLeds_( FlashingLeds( &ledOutput_ ) ),
        pulsingLeds_( PulsingLeds( &ledOutput_ ) ),
        grid_( Grid( &buttonInput_, &ledOutput_, &flashingLeds_, &pulsingLeds_ ) )
{
}

GridInterface& GridContainer::getGrid()
{
    return grid_;
}

}