#ifndef APPLICATION_INTERNAL_MENU_H_
#define APPLICATION_INTERNAL_MENU_H_

#include "application/Application.h"
#include "thread.hpp"

namespace grid
{
    class Grid;
    class AdditionalButtons;
}

namespace lcd
{
    class Lcd;
}

namespace mcu
{
    class System;
}

namespace application
{

class InternalMenu : public Application
{
public:
    InternalMenu( ApplicationController& applicationController, grid::Grid& grid, grid::AdditionalButtons& additionalButtons,
        lcd::Lcd& lcd, mcu::System& system );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event );
    void handleGridButtonEvent( const grid::Grid::ButtonEvent event );

    void displayMessageOnLcd();

    grid::Grid& grid_;
    lcd::Lcd& lcd_;
    mcu::System& system_;
    ApplicationIndex applicationToFollow_;
};

} // namespace

#endif // APPLICATION_INTERNAL_MENU_H_