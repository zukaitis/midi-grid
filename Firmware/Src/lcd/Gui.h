#ifndef LCD_GUI_H_
#define LCD_GUI_H_

#include "program/Launchpad.h"

namespace launchpad
{
    class Launchpad;
}

namespace lcd
{

class Lcd;

namespace gui
{

static const uint16_t TIMEOUT_CHECK_STEP = 250; // ms

static const uint16_t MIDI_TIMEOUT = 1250; // ms
static const uint16_t ROTARY_CONTROL_TIMEOUT = 1250; // ms

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

    void displayConnectingImage();
    void displayRotaryControlValues( const uint8_t value1, const uint8_t value2 );
    void displayWaitingForMidi();
    void enterLaunchpad95Mode();
    void enterInternalMenu();
    void registerMidiInputActivity();
    void registerMidiOutputActivity();
    void refresh();
    void refreshStatusBar();
    void refreshMainArea();
    void setDawClipName( const char* const name, const uint8_t length );
    void setDawDeviceName( const char* const name, const uint8_t length );
    void setDawStatus( const bool isPlaying, const bool isRecording, const bool isSessionRecording );
    void setDawTimingValues( const uint16_t tempo, const uint8_t signatureNumerator, const uint8_t signatureDenominator,
            const bool nudgeDown, const bool nudgeUp );
    void setDawTrackName( const char* const name, const uint8_t length );
    void setLaunchpad95Mode( const launchpad::Launchpad95Mode mode );
    void setLaunchpad95Submode( const launchpad::Launchpad95Submode submode );

private:
    void displayClipName();
    void displayDeviceName();
    void displayLaunchpad95Info();
    void displayLaunchpad95Mode();
    void displayLaunchpad95Submode();
    void displayStatus();
    void displayTimingStatus();
    void displayTrackName();

    lcd::Lcd& lcd;

    char dawClipName_[15] = " \0";
    char dawDeviceName_[15] = " \0";
    char dawTrackName_[15] = " \0";
    bool dawIsPlaying_ = false;
    bool dawIsRecording_ = false;
    bool dawIsSessionRecording_ = false;
    bool dawNudgeDownActive_ = false;
    bool dawNudgeUpActive_ = false;
    uint16_t dawTempo_ = 0;
    uint8_t dawSignatureNumerator_ = 0;
    uint8_t dawSignatureDenominator_ = 0;

    launchpad::Launchpad95Mode launchpad95Mode_ = launchpad::Launchpad95Mode_UNKNOWN;
    launchpad::Launchpad95Submode launchpad95Submode_ = launchpad::Launchpad95Submode_DEFAULT;
    uint16_t midiInputTimeout_ = 0;
    uint16_t midiOutputTimeout_ = 0;
    uint16_t rotaryControlDisplayTimeout_ = 0;
    bool statusBarActive_ = false;
};

} // namespace gui
} // namespace lcd

#endif /* LCD_GUI_H_ */
