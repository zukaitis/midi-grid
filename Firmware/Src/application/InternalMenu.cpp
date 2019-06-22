#include "application/InternalMenu.hpp"

#include "grid/Grid.hpp"
#include "grid/AdditionalButtons.hpp"
#include "lcd/Lcd.hpp"
#include "system/System.hpp"

#include "Types.h"

namespace application {

static const uint8_t kBootloaderButtonX = 7;
static const uint8_t kBootloaderButtonY = 0;
static const Color kBootloaderButtonColor = {64U, 0U, 0U};

static const uint8_t kSnakeButtonX = 0;
static const uint8_t kSnakeButtonY = 7;
static const Color kSnakeButtonColor = {0U, 64U, 0U};

static const uint8_t kLaunchpadButtonX = 1;
static const uint8_t kLaunchpadButtonY = 7;
static const Color kLaunchpadButtonColor = {0U, 64U, 64U};

static const uint8_t kGridTestButtonX = 2;
static const uint8_t kGridTestButtonY = 7;
static const Color kGridTestButtonColor = {64U, 64U, 0U};

InternalMenu::InternalMenu( ApplicationController& applicationController, grid::Grid& grid, grid::AdditionalButtons& additionalButtons,
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

void InternalMenu::handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event )
{
    if ((grid::AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_RELEASED == event.action))
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
