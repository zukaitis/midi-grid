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

static const char launchpad95SubmodeString[9][15] = {
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

    void setLaunchpad95Mode( launchpad::Launchpad95Mode mode );
    void setLaunchpad95Submode( launchpad::Launchpad95Submode submode );
    void setTrackName(char* name, uint8_t length);
    void setClipName(char* name, uint8_t length);
    void setDeviceName(char* name, uint8_t length);
    void setStatus(const bool isPlaying, const bool isRecording, const bool isSessionRecording);
    void setTimingValues( const uint16_t tempo, const uint8_t signatureNumerator, const uint8_t signatureDenominator,
            const bool nudgeDown, const bool nudgeUp );
    void displayUsbLogo();
    void displayStatusBar();

    void refresh();
    void refreshStatusBar();

    void registerMidiInputActivity();
    void registerMidiOutputActivity();

private:
    void displayLaunchpad95Info();

    void displayLaunchpad95Mode();
    void displayLaunchpad95Submode();

    void displayTrackName();
    void displayClipName();
    void displayDeviceName();
    void displayStatus();
    void displayTimingStatus();


    lcd::Lcd& lcd;
    bool statusBarActive = false;

    launchpad::Launchpad95Mode launchpad95Mode = launchpad::Launchpad95Mode_UNKNOWN;
    launchpad::Launchpad95Submode launchpad95Submode = launchpad::Launchpad95Submode_DEFAULT;

    bool isPlaying_ = false;
    bool isRecording_ = false;
    bool isSessionRecording_ = false;

    uint16_t tempo_ = 0;
    uint8_t signatureNumerator_ = 0;
    uint8_t signatureDenominator_ = 0;
    bool nudgeDownActive_ = false;
    bool nudgeUpActive_ = false;

    char trackName[15] = " \0";
    char clipName[15] = " \0";
    char deviceName[15] = " \0";

    uint16_t midiInputTimeout = 0;
    uint16_t midiOutputTimeout = 0;
};

} // namespace

#endif /* LCD_GUI_H_ */
