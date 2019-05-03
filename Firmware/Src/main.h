#ifndef __MAIN_H__
#define __MAIN_H__

#include "system/System.h"
#include "grid/GridDriver.h"
#include "grid/Grid.h"
#include "grid/AdditionalButtons.h"
#include "grid/RotaryControls.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "application/InternalMenu.h"
#include "application/Launchpad.h"
#include "application/Startup.h"
#include "application/GridTest.h"
#include "system/GlobalInterrupts.h"
#include "usb/UsbMidi.h"

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
    lcd::Gui gui_;
    application::ApplicationController applicationController_;
    application::Startup startup_;
    application::GridTest gridTest_;
    application::InternalMenu internalMenu_;
    application::Launchpad launchpad_;
};

#endif // __MAIN_H__
