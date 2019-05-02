#include "application/Startup.h"

#include "grid/GridDriver.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "system/System.h"

namespace application
{

Startup::Startup( grid::GridDriver& gridDriver, lcd::Gui& gui, lcd::Lcd& lcd, mcu::System& system ):
    gridDriver_( gridDriver ),
    gui_( gui ),
    lcd_( lcd ),
    system_( system )
{
}

void Startup::initialize()
{
    lcd_.initialize();
    lcd_.setBacklightIntensity( 60 );

    gridDriver_.initialize();
    gridDriver_.start();

    gui_.displayConnectingImage();

    while (!system_.isUsbConnected())
    {
    }

    gui_.displayWaitingForMidi();
    
    switchApplication( ApplicationIndex_GRID_TEST );
}

} // namespace
