#ifndef APPLICATION_INTERNAL_MENU_HPP_
#define APPLICATION_INTERNAL_MENU_HPP_

#include "application/Application.hpp"
#include "freertos/thread.hpp"

namespace grid
{
    class GridInterface;
    class AdditionalButtonsInterface;
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
    InternalMenu( ApplicationController& applicationController, grid::GridInterface& grid, additional_buttons::AdditionalButtonsInterface& additionalButtons,
        lcd::Lcd& lcd, mcu::System& system );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const additional_buttons::Event event );
    void handleGridButtonEvent( const grid::ButtonEvent event );

    void updateLcd() const;

    grid::GridInterface& grid_;
    lcd::Lcd& lcd_;
    mcu::System& system_;
    ApplicationIndex applicationToFollow_;
};

} // namespace

#endif // APPLICATION_INTERNAL_MENU_HPP_