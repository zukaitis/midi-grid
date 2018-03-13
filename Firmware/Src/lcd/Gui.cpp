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
    lcd.print("              ", 0, 32); // clear that portion of the lcd
    if (launchpad::Launchpad95Mode_UNKNOWN != mode)
    {
        lcd.print(launchpad95ModeString[mode], lcd::WIDTH/2, 32, lcd::Justification_CENTER);
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
    lcd.print( "L95", 0, 0 );

    lcd.print( "I:", 24, 0 );
    lcd.displayImage(35, 0, lcd::circle);

    lcd.print( "O:", 44, 0 );
    lcd.displayImage(55, 0, lcd::circle);

    lcd.print( "E:", 64, 0 );
    lcd.displayImage(75, 0, lcd::circle);

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
    uint32_t i = HAL_GetTick();
    //if (HAL_GetTick() >= refreshCheckTime)
    {
#if 0
        if (midiInputTimeout > 0)
        {
            midiInputTimeout -= MIDI_TIMEOUT_STEP;
            do
            {
                if (midiInputTimeout > MIDI_TIMEOUT_FULL_CIRCLE)
                {
                    lcd.displayImage(35, 0, lcd::circleFull);
                    break;
                }
                if (midiInputTimeout > MIDI_TIMEOUT_BIG_DOT)
                {
                    lcd.displayImage(35, 0, lcd::circleBigDot);
                    break;
                }
                if (midiInputTimeout > 0)
                {
                    lcd.displayImage(35, 0, lcd::circleSmallDot);
                }
                else
                {
                    lcd.displayImage(35, 0, lcd::circle);
                }
            } while (false);
        }

        if (midiOutputTimeout > 0)
        {
            midiOutputTimeout -= MIDI_TIMEOUT_STEP;
            do
            {
                if (midiOutputTimeout > MIDI_TIMEOUT_FULL_CIRCLE)
                {
                    lcd.displayImage(55, 0, lcd::circleFull);
                    break;
                }
                if (midiOutputTimeout > MIDI_TIMEOUT_BIG_DOT)
                {
                    lcd.displayImage(55, 0, lcd::circleBigDot);
                    break;
                }
                if (midiOutputTimeout > 0)
                {
                    lcd.displayImage(55, 0, lcd::circleSmallDot);
                }
                else
                {
                    lcd.displayImage(55, 0, lcd::circle);
                }
            } while (false);
        }

        if (midiExternalTimeout > 0)
        {
            midiExternalTimeout -= MIDI_TIMEOUT_STEP;
            do
            {
                if (midiExternalTimeout > MIDI_TIMEOUT_FULL_CIRCLE)
                {
                    lcd.displayImage(75, 0, lcd::circleFull);
                    break;
                }
                if (midiExternalTimeout > MIDI_TIMEOUT_BIG_DOT)
                {
                    lcd.displayImage(75, 0, lcd::circleBigDot);
                    break;
                }
                if (midiExternalTimeout > 0)
                {
                    lcd.displayImage(75, 0, lcd::circleSmallDot);
                }
                else
                {
                    lcd.displayImage(75, 0, lcd::circle);
                }
            } while (false);
        }
#endif
        refreshCheckTime = HAL_GetTick() + 10000; //MIDI_TIMEOUT_STEP; // check every 250ms
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
