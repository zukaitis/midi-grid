#ifndef LCD_GUI_H_
#define LCD_GUI_H_

#include "program/Launchpad.h"

namespace hal {
class Time;
}

namespace lcd
{

class Lcd;

class Gui
{
public:
    Gui( Lcd& lcd, hal::Time& time );
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

    lcd::Lcd& lcd_;
    hal::Time& time_;

    char dawClipName_[15];
    char dawDeviceName_[15];
    char dawTrackName_[15];
    bool dawIsPlaying_;
    bool dawIsRecording_;
    bool dawIsSessionRecording_;
    bool dawNudgeDownActive_;
    bool dawNudgeUpActive_;
    uint16_t dawTempo_;
    uint8_t dawSignatureNumerator_;
    uint8_t dawSignatureDenominator_;

    launchpad::Launchpad95Mode launchpad95Mode_;
    launchpad::Launchpad95Submode launchpad95Submode_;
    uint16_t midiInputTimeout_;
    uint16_t midiOutputTimeout_;
    uint16_t rotaryControlDisplayTimeout_;
    bool statusBarActive_;
};

} // namespace lcd

#endif /* LCD_GUI_H_ */
