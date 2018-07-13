#ifndef __MAIN_H__
#define __MAIN_H__

#include "grid/Grid.h"
#include "grid/GridControl.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "program/Launchpad.h"
#include "usb/UsbMidi.h"

//namespace grid
//{
//    namespace grid_control
//    {
//        class GridControl;
//    }
//}

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

    grid::grid_control::GridControl gridControl_;
    grid::Grid grid_;
    grid::switches::Switches switches_;
    lcd::gui::Gui gui_;
    midi::UsbMidi usbMidi_;
    launchpad::Launchpad launchpad_;
    lcd::Lcd& lcd_; // to be removed
};

#endif // __MAIN_H__
