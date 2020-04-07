#include "application/internal_menu/InternalMenu.hpp"

#include "grid/GridInterface.h"
#include "additional_buttons/AdditionalButtonsInterface.h"
#include "lcd/LcdInterface.h"
#include "system/System.hpp"

#include "types/Color.h"

namespace application {

static const uint8_t kBootloaderButtonX = 7;
static const uint8_t kBootloaderButtonY = 0;
static const Coordinates kBootloaderButton = {7,0};
static const Color kBootloaderButtonColor = color::RED;

static const uint8_t kSnakeButtonX = 0;
static const uint8_t kSnakeButtonY = 7;
static const Coordinates kSnakeButton = {0,7};
static const Color kSnakeButtonColor = color::SPRING_GREEN;

static const uint8_t kLaunchpadButtonX = 1;
static const uint8_t kLaunchpadButtonY = 7;
static const Coordinates kLaunchpadButton = {1,7};
static const Color kLaunchpadButtonColor = color::ORANGE;

static const uint8_t kGridTestButtonX = 2;
static const uint8_t kGridTestButtonY = 7;
static const Coordinates kGridTestButton = {2,7};
static const Color kGridTestButtonColor = color::YELLOW;

InternalMenu::InternalMenu( ApplicationController& applicationController, grid::GridInterface& grid, additional_buttons::AdditionalButtonsInterface& additionalButtons,
    lcd::LcdInterface& lcd, mcu::System& system ):
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
    lcd_.release(); // internal menu turns syslog view off

    grid_.turnAllLedsOff();
    grid_.setLed( {kBootloaderButtonX, kBootloaderButtonY}, kBootloaderButtonColor );
    grid_.setLed( {kSnakeButtonX, kSnakeButtonY}, kSnakeButtonColor );
    grid_.setLed( {kLaunchpadButtonX, kLaunchpadButtonY}, kLaunchpadButtonColor );
    grid_.setLed( {kGridTestButtonX, kGridTestButtonY}, kGridTestButtonColor );

    applicationToFollow_ = ApplicationIndex_PREVIOUS;

    enableGridInputHandler();
    enableAdditionalButtonInputHandler();
}

void InternalMenu::handleAdditionalButtonEvent( const additional_buttons::Event& event )
{
    if ((additional_buttons::Button::internalMenu == event.button) && (ButtonAction::RELEASED == event.action))
    {
        switchApplication( applicationToFollow_ );
    }
}

void InternalMenu::handleGridButtonEvent( const grid::ButtonEvent& event )
{
    if (ButtonAction::PRESSED == event.action)
    {
        // TODO(unknown): replace this with switch case
        if ((kBootloaderButton == event.coordinates))
        {
            // reset into DFU bootloader
            system_.resetIntoBootloader();
        }
        else if (kSnakeButton == event.coordinates)
        {
            applicationToFollow_ = ApplicationIndex_SNAKE;
        }
        else if (kLaunchpadButton == event.coordinates)
        {
            applicationToFollow_ = ApplicationIndex_LAUNCHPAD;
        }
        else if (kGridTestButton == event.coordinates)
        {
            applicationToFollow_ = ApplicationIndex_GRID_TEST;
        }
    }
}

void InternalMenu::updateLcd() const
{
    lcd_.clear();
    lcd_.print( "Internal Menu", 16, lcd::Justification::CENTER );
    lcd_.print( "Active", 24, lcd::Justification::CENTER );
}

}
