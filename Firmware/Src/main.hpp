#ifndef MAIN_HPP_
#define MAIN_HPP_

#include "system/System.hpp"
#include "grid/GridDriver.hpp"
#include "grid/Grid.hpp"
#include "grid/AdditionalButtons.hpp"
#include "grid/RotaryControls.hpp"
#include "lcd/Lcd.hpp"
#include "application/internal_menu/InternalMenu.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/startup/Startup.hpp"
#include "application/grid_test/GridTest.hpp"
#include "application/snake/Snake.hpp"
#include "system/GlobalInterrupts.hpp"
#include "usb/UsbMidi.hpp"

#include "thread.hpp"

class Main
{
public:
    static inline Main& getInstance()
    {
        static Main instance;
        return instance;
    }

    void run();
private:
    Main();

    mcu::System system_;
    mcu::GlobalInterrupts globalInterrupts_;
    grid::GridDriver gridDriver_;
    grid::Grid grid_;
    grid::AdditionalButtons additionalButtons_;
    grid::RotaryControls rotaryControls_;
    midi::UsbMidi usbMidi_;
    lcd::Lcd lcd_;
    application::ApplicationController applicationController_;
    application::Startup startup_;
    application::GridTest gridTest_;
    application::InternalMenu internalMenu_;
    application::launchpad::Launchpad launchpad_;
    application::Snake snake_;
};

#endif // MAIN_HPP_
