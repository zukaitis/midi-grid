#include "application/startup/Startup.hpp"

#include "hardware/grid/GridDriver.h"
#include "io/lcd/Lcd.hpp"
#include "application/images.h"
#include "system/System.hpp"

namespace application
{

static const lcd::Image usbLogo = { usbLogoArray, 60, 24 };

Startup::Startup( ApplicationController& applicationController, hardware::grid::GridDriver& gridDriver, lcd::Lcd& lcd, mcu::System& system ):
        Application( applicationController ),
        gridDriver_( gridDriver ),
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

    displayUsbConnecting();

    while (!system_.isUsbConnected())
    {
    }

    switchApplication( ApplicationIndex_GRID_TEST );
}

void Startup::displayUsbConnecting()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, usbLogo );
    lcd_.print( "USB Connecting", lcd_.horizontalCenter, 40, lcd::Justification_CENTER );
}

} // namespace
