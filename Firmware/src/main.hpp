#pragma once

#include "system/System.hpp"
#include "hardware/grid/GridDriver.h"

#include "grid/GridContainer.h"
#include "additional_buttons/AdditionalButtons.h"
#include "rotary_controls/RotaryControls.h"

#include "hardware/lcd/Spi.h"
#include "hardware/lcd/BacklightDriver.h"
#include "lcd/LcdContainer.h"
#include "syslog/LogContainer.h"

#include "application/internal_menu/InternalMenu.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/startup/Startup.hpp"
#include "application/grid_test/GridTest.hpp"
#include "application/snake/Snake.hpp"

#include "system/GlobalInterrupts.hpp"
#include "testing/Testing.h"
#include "usb/UsbMidi.hpp"

#include <freertos/thread.hpp>

class Main
{
public:
    static inline Main& getInstance()
    {
        static Main instance;
        return instance;
    }

    syslog::LogContainer& getLogContainer();

    void run();
private:
    Main();

    mcu::System system_;
    mcu::GlobalInterrupts globalInterrupts_;
    hardware::grid::GridDriver gridDriver_;
    grid::GridContainer gridContainer_;
    additional_buttons::AdditionalButtons additionalButtons_;
    rotary_controls::RotaryControls rotaryControls_;
    midi::UsbMidi usbMidi_;
    hardware::lcd::Spi lcdSpi_;
    hardware::lcd::BacklightDriver backlightDriver_;
    lcd::LcdContainer lcdContainer_;
    testing::Testing testing_;
    syslog::LogContainer logContainer_;
    application::ApplicationController applicationController_;
    application::Startup startup_;
    application::GridTest gridTest_;
    application::InternalMenu internalMenu_;
    application::launchpad::Launchpad launchpad_;
    application::Snake snake_;
};
