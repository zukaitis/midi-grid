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
#include "system/GlobalInterrupts.h"
#include "usb/UsbMidi.h"

#include "thread.hpp"

class ApplicationMain: public freertos::Thread
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
    virtual void Run();

    void randomLightAnimation();
    void runGridInputTest();
    void runInternalMenu();

    void switchApplicationCallback( const uint8_t applicationIndex );

    void displayBootAnimation();

private:
    ApplicationMain();

    mcu::System system_;
    mcu::GlobalInterrupts globalInterrupts_;
    grid::GridDriver gridDriver_;
    grid::Grid grid_;
    grid::AdditionalButtons additionalButtons_;
    grid::RotaryControls rotaryControls_;
    midi::UsbMidi usbMidi_;
    lcd::Lcd lcd_;
    lcd::Gui gui_;
    launchpad::Launchpad launchpad_;
    internal_menu::InternalMenu internalMenu_;

    bool internalMenuRunning; // temporary variable, to be removed
};

#endif // __MAIN_H__
