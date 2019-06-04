#include "application/InternalMenu.h"

#include "grid/Grid.h"
#include "grid/AdditionalButtons.h"
#include "lcd/Gui.h"
#include "system/System.h"

#include "Types.h"

namespace application {

static const uint8_t kBootloaderButtonX = 7;
static const uint8_t kBootloaderButtonY = 0;

InternalMenu::InternalMenu( ApplicationController& applicationController, grid::Grid& grid, grid::AdditionalButtons& additionalButtons,
    lcd::Gui& gui, mcu::System& system ):
        Application( applicationController ),
        grid_( grid ),
        gui_( gui ),
        system_( system )
{
}

void InternalMenu::run( ApplicationController& thread )
{
    grid_.discardAllPendingButtonEvents();
    grid_.turnAllLedsOff();
    gui_.enterInternalMenu();

    static const Color red = {64U, 0U, 0U};
    grid_.setLed( kBootloaderButtonX, kBootloaderButtonY, red );

    enableGridInputHandler();
    enableAdditionalButtonInputHandler();
}

void InternalMenu::handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event )
{
    if ((grid::AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_RELEASED == event.action))
    {
        switchApplication( ApplicationIndex_PREVIOUS );
    }
}

void InternalMenu::handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    if ((kBootloaderButtonX == event.positionX) && (kBootloaderButtonY == event.positionX) && (ButtonAction_PRESSED == event.action))
    {
        // reset into DFU bootloader
        system_.resetIntoBootloader();
    }
}

}
