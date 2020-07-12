#include "application/startup/Startup.hpp"

#include "hardware/grid/GridDriver.h"
#include "lcd/LcdInterface.h"
#include "application/images.h"
#include "lcd/text/Format.h"
#include "system/System.hpp"
#include "types/Color.h"

namespace application
{

static const lcd::ImageLegacy usbLogo = { &usbLogoArray[0], 60, 24 };

Startup::Startup( ApplicationController& applicationController, hardware::grid::GridDriver& gridDriver, lcd::LcdInterface& lcd, mcu::System& system ):
        Application( applicationController ),
        gridDriver_( gridDriver ),
        lcd_( lcd ),
        system_( system )
{
}

void Startup::run( ApplicationThread& thread )
{
    lcd_.initialize();
    lcd_.backlight().setIntensity( 60 );

    gridDriver_.initialize();
    gridDriver_.start();

    displayUsbConnecting();

    while (!system_.isUsbConnected())
    {
    }

    switchApplication( ApplicationIndex_GRID_TEST );
}

void Startup::displayUsbConnecting()
{
    lcd_.image().createNew( color::BLACK );
    lcd_.text().print( "USB Connecting", 120, lcd::Format().justification( lcd::Justification::CENTER ) );
    lcd_.image().display();
}

}  // namespace application
