#ifndef __MAIN_H__
#define __MAIN_H__

#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "program/Launchpad.h"
#include "usb/UsbMidi.h"

namespace lcd
{
    class Lcd;
}

class ApplicationMain
{
public:

    // singleton, because class has to be called from main() function
    static ApplicationMain& getInstance()
    {
        static ApplicationMain instance;
        return instance;
    }

    ~ApplicationMain();

    void initialize();
    void run();

    void configureNvicPriorities();
    void configureSystemClock();
    void randomLightAnimation();
    void resetIntoBootloader();
    void runInternalMenu();

private:
    ApplicationMain();

    grid::Grid grid;
    grid::switches::Switches switches;
    lcd::gui::Gui gui;
    midi::UsbMidi usbMidi;
    launchpad::Launchpad launchpad;
    lcd::Lcd& lcd_; // to be removed
};

#endif // __MAIN_H__
