#include "application/Startup.hpp"

#include "grid/GridDriver.hpp"
#include "lcd/Gui.hpp"
#include "lcd/Lcd.hpp"
#include "system/System.hpp"

namespace application
{

Startup::Startup( ApplicationController& applicationController, grid::GridDriver& gridDriver,
    lcd::Gui& gui, lcd::Lcd& lcd, mcu::System& system ):
        Application( applicationController ),
        gridDriver_( gridDriver ),
        gui_( gui ),
        lcd_( lcd ),
        system_( system )
{
}

void Startup::run( ApplicationThread& thread )
{
    lcd_.initialize();
    lcd_.setBacklightIntensity( 60 );

    gridDriver_.initialize();
    gridDriver_.start();

    gui_.displayConnectingImage();

    while (!system_.isUsbConnected())
    {
    }

    switchApplication( ApplicationIndex_GRID_TEST );
}

} // namespace
