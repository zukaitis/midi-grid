#include "application/internal_menu/InternalMenu.hpp"

#include "io/grid/Grid.hpp"
#include "io/AdditionalButtons.hpp"
#include "io/lcd/Lcd.hpp"
#include "system/System.hpp"

#include "types/Color.h"

namespace application {

static const uint8_t kBootloaderButtonX = 7;
static const uint8_t kBootloaderButtonY = 0;
static const Color kBootloaderButtonColor = color::RED;

static const uint8_t kSnakeButtonX = 0;
static const uint8_t kSnakeButtonY = 7;
static const Color kSnakeButtonColor = color::SPRING_GREEN;

static const uint8_t kLaunchpadButtonX = 1;
static const uint8_t kLaunchpadButtonY = 7;
static const Color kLaunchpadButtonColor = color::ORANGE;

static const uint8_t kGridTestButtonX = 2;
static const uint8_t kGridTestButtonY = 7;
static const Color kGridTestButtonColor = color::YELLOW;

InternalMenu::InternalMenu( ApplicationController& applicationController, grid::Grid& grid, AdditionalButtons& additionalButtons,
    lcd::Lcd& lcd, mcu::System& system ):
        Application( applicationController ),
        grid_( grid ),
        lcd_( lcd ),
        system_( system ),
        applicationToFollow_( ApplicationIndex_PREVIOUS )
{
}

void InternalMenu::run( ApplicationThread& thread )
{
    updateLcd();

    grid_.turnAllLedsOff();
    grid_.setLed( kBootloaderButtonX, kBootloaderButtonY, kBootloaderButtonColor );
    grid_.setLed( kSnakeButtonX, kSnakeButtonY, kSnakeButtonColor );
    grid_.setLed( kLaunchpadButtonX, kLaunchpadButtonY, kLaunchpadButtonColor );
    grid_.setLed( kGridTestButtonX, kGridTestButtonY, kGridTestButtonColor );

    applicationToFollow_ = ApplicationIndex_PREVIOUS;

    enableGridInputHandler();
    enableAdditionalButtonInputHandler();
}

void InternalMenu::handleAdditionalButtonEvent( const AdditionalButtons::Event event )
{
    if ((AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_RELEASED == event.action))
    {
        switchApplication( applicationToFollow_ );
    }
}

void InternalMenu::handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    if (ButtonAction_PRESSED == event.action)
    {
        if ((kBootloaderButtonX == event.positionX) && (kBootloaderButtonY == event.positionY))
        {
            // reset into DFU bootloader
            system_.resetIntoBootloader();
        }
        else if ((kSnakeButtonX == event.positionX) && (kSnakeButtonY == event.positionY))
        {
            applicationToFollow_ = ApplicationIndex_SNAKE;
        }
        else if ((kLaunchpadButtonX == event.positionX) && (kLaunchpadButtonY == event.positionY))
        {
            applicationToFollow_ = ApplicationIndex_LAUNCHPAD;
        }
        else if ((kGridTestButtonX == event.positionX) && (kGridTestButtonY == event.positionY))
        {
            applicationToFollow_ = ApplicationIndex_GRID_TEST;
        }
    }
}

void InternalMenu::updateLcd() const
{
    lcd_.clear();
    lcd_.print( "Internal Menu", lcd_.horizontalCenter, 16, lcd::Justification_CENTER );
    lcd_.print( "Active", lcd_.horizontalCenter, 24, lcd::Justification_CENTER );
}

}
