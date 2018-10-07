#ifndef __MAIN_H__
#define __MAIN_H__

#include "grid/GridDriver.h"
#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "application/Launchpad.h"
#include "hal/GlobalInterrupts.h"
#include "hal/Hal.h"
#include "hal/Time.h"
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

    hal::Hal hal_;
    hal::GlobalInterrupts globalInterrupts_;
    hal::Time time_;
    grid::GridDriver gridDriver_;
    grid::Grid grid_;
    grid::Switches switches_;
    midi::UsbMidi usbMidi_;
    lcd::Lcd lcd_;
    lcd::Gui gui_;
    launchpad::Launchpad launchpad_;
};

#endif // __MAIN_H__
