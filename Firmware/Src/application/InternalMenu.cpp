#include "application/InternalMenu.h"

#include "grid/Grid.h"
#include "grid/AdditionalButtons.h"
#include "lcd/Gui.h"
#include "system/System.h"

#include "Types.h"

namespace internal_menu {

static const uint8_t kBootloaderButtonX = 7;
static const uint8_t kBootloaderButtonY = 0;

AdditionalButtonInputHandler::AdditionalButtonInputHandler( grid::AdditionalButtons& additionalButtons, std::function<void()> stopApplicationCallback ):
    Thread( "InteralMenu_AdditionalButtonInputHandler", 500, 3 ),
    additionalButtons_( additionalButtons ),
    stopApplication_( stopApplicationCallback )
{
}

void AdditionalButtonInputHandler::Run()
{
    while (true)
    {
        Delay( 10 ); // to be replaced with block from additional button queue

        grid::AdditionalButtons::Event event = {};
        if (additionalButtons_.waitForEvent( event ))
        {
            if ((grid::AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_PRESSED == event.action))
            {
                stopApplication_(); // return from interal menu
            }
        }
    }
}

GridInputHandler::GridInputHandler( grid::Grid& grid, mcu::System& system ):
    Thread( "InteralMenu_GridInputHandler", 500, 3 ),
    grid_( grid ),
    system_( system )
{
}

void GridInputHandler::Run()
{
    while (true)
    {
        grid::Grid::ButtonEvent event = {};
        Delay( 10 ); // to be replaced with block from additional button queue
        if (grid_.waitForButtonEvent( event ))
        {
            if ((kBootloaderButtonX == event.positionX) && (kBootloaderButtonY == event.positionX) && (ButtonAction_PRESSED == event.action))
            {
                // reset into DFU bootloader
                system_.resetIntoBootloader();
            }
        }
    }
}

InternalMenu::InternalMenu( grid::Grid& grid, grid::AdditionalButtons& additionalButtons, lcd::Gui& gui, mcu::System& system, std::function<void(const uint8_t)> switchApplicationCallback ):
        ApplicationBase(),
        grid_( grid ),
        gui_( gui ),
        additionalButtonInputHandler_( AdditionalButtonInputHandler( additionalButtons, std::bind( &InternalMenu::stopApplicationCallback, this ) ) ),
        gridInputHandler_( GridInputHandler( grid, system )),
        switchApplication_( switchApplicationCallback )
{
    initializeInputHandlers( { &additionalButtonInputHandler_, &gridInputHandler_ } );
}

void InternalMenu::initialize()
{
    grid_.discardAllPendingButtonEvents();
    // additionalButtons_.discardAllPendingEvents();
    grid_.turnAllLedsOff();
    gui_.enterInternalMenu();

    static const Color red = {64U, 0U, 0U};
    grid_.setLed( kBootloaderButtonX, kBootloaderButtonY, red );
}

void InternalMenu::stopApplicationCallback()
{
    switchApplication_( 0 );
}

}
