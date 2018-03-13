/*
 * gui.h
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */

#ifndef LCD_GUI_H_
#define LCD_GUI_H_

//#include "lcd/Lcd.h"
#include "program/Launchpad.h"

namespace lcd
{
    class Lcd;
}

namespace launchpad
{
    class Launchpad;
}

namespace gui
{

static const uint16_t MIDI_TIMEOUT_STEP = 250; // ms
static const uint16_t MIDI_TIMEOUT = 2500; // ms - 3*750ms + 250ms
static const uint16_t MIDI_TIMEOUT_FULL_CIRCLE = 1750; // ms - 2*750ms
static const uint16_t MIDI_TIMEOUT_BIG_DOT = 1000; // ms - 3*750ms + 250ms

static const char launchpad95ModeString[8][16] = {
        "Session\0",
        "Instrument\0",
        "Device control\0",
        "User 1\0",
        "Drum step seq.\0",
        "Melodic seq.\0",
        "User 2\0",
        "Mixer\0"
};

class Gui
{
public:
    Gui();
    ~Gui();

    void displayLaunchpad95Mode( launchpad::Launchpad95Mode mode );
    void gui_changeLaunchpad95Submode();
    void displayUsbLogo();
    void displayStatusBar();

    void refresh();
    void refreshStatusBar();

    void registerMidiInputActivity();
    void registerMidiOutputActivity();
    void registerMidiExternalActivity();

private:
    lcd::Lcd& lcd;
    bool statusBarActive = false;

    uint16_t midiInputTimeout = 0;
    uint16_t midiOutputTimeout = 0;
    uint16_t midiExternalTimeout = 0;

};

} // namespace

#endif /* LCD_GUI_H_ */
