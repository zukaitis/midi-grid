/*
 * gui.c
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */
#include "lcd/Gui.h"
#include "lcd/Lcd.h"

#include "stm32f4xx_hal.h"

namespace gui
{

Gui::Gui() : lcd( lcd::Lcd::getInstance() )
{}

Gui::~Gui()
{}

void Gui::displayLaunchpad95Mode( launchpad::Launchpad95Mode mode )
{
    lcd.clearArea(0, 8, 83, 15);
    if (launchpad::Launchpad95Mode_UNKNOWN != mode)
    {
        lcd.print(launchpad95ModeString[mode], lcd::WIDTH/2, 8, lcd::Justification_CENTER);
    }
}

void Gui::gui_changeLaunchpad95Submode()
{

}

void Gui::displayUsbLogo()
{
    lcd.displayImage(0, 0, lcd::usbLogo);
}

void Gui::displayStatusBar()
{
    lcd.clearArea(0, 0, 83, 7);
    lcd.print( "L95", 0, 0 );
    lcd.displayImage(63, 0, lcd::usbSymbolSmall);

    statusBarActive = true;
}

void Gui::refresh()
{
    if (statusBarActive)
    {
        refreshStatusBar();
    }

    lcd.refresh();
}



void Gui::refreshStatusBar()
{
    static uint32_t refreshCheckTime = 0;

    if (HAL_GetTick() >= refreshCheckTime)
    {
        if (midiInputTimeout > 0)
        {
            midiInputTimeout -= MIDI_TIMEOUT_STEP;
            if (midiInputTimeout > 0)
            {
                lcd.displayImage(73, 0, lcd::arrowSmallDown);
            }
            else
            {
                lcd.clearArea(73, 0, 77, 7);
            }
        }

        if (midiOutputTimeout > 0)
        {
            midiOutputTimeout -= MIDI_TIMEOUT_STEP;
            if (midiOutputTimeout > 0)
            {
                lcd.displayImage(78, 0, lcd::arrowSmallUp);
            }
            else
            {
                lcd.clearArea(78, 0, 83, 7);
            }
        }

        refreshCheckTime = HAL_GetTick() + MIDI_TIMEOUT_STEP; // check every 250ms
    }
}

void Gui::registerMidiInputActivity()
{
    midiInputTimeout = MIDI_TIMEOUT;
}

void Gui::registerMidiOutputActivity()
{
    midiOutputTimeout = MIDI_TIMEOUT;
}

void Gui::registerMidiExternalActivity()
{
    midiExternalTimeout = MIDI_TIMEOUT;
}

} // namespace
