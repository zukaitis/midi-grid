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
static const uint16_t MIDI_TIMEOUT = 1250; // ms

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

static const char launchpad95SubmodeString[9][16] = {
        " \0",
        "Scale\0",
        "Volume\0",
        "Pan\0",
        "Send A\0",
        "Send B\0",
        "MSS: Length\0",
        "MSS: Octave\0",
        "MSS: Velocity\0"
};

class Gui
{
public:
    Gui();
    ~Gui();

    void displayLaunchpad95Mode( launchpad::Launchpad95Mode mode );
    void displayLaunchpad95Submode( launchpad::Launchpad95Submode submode );
    void displayTrackName(char* name, uint8_t length);
    void displayClipName(char* name, uint8_t length);
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
