#ifndef __MAIN_H__
#define __MAIN_H__

#include <grid/GridControl.h>
#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "program/Launchpad.h"
#include "system/GlobalInterrupts.h"
#include "system/System.h"
#include "system/Time.h"
#include "usb/UsbMidi.h"

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

    Colour getRandomColour();
    void randomLightAnimation();
    void runGridInputTest();
    void runInternalMenu();

private:
    ApplicationMain();

    System system_;
    GlobalInterrupts globalInterrupts_;
    Time time_;
    grid::GridControl gridControl_;
    grid::Grid grid_;
    grid::Switches switches_;
    midi::UsbMidi usbMidi_;
    lcd::Lcd lcd_;
    lcd::Gui gui_;
    launchpad::Launchpad launchpad_;
};

#endif // __MAIN_H__
