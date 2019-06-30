
#include "application/launchpad/LcdGui.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/launchpad/Images.hpp"

#include "io/lcd/Lcd.hpp"
#include "ticks.hpp"

#include <cstdio>

namespace application
{
namespace launchpad
{

static const lcd::Image usbSymbolSmall = { usbSymbolSmallArray, 9, 8 };
static const lcd::Image midiSymbolSmall = { midiSymbolSmallArray, 8, 8 };
static const lcd::Image arrowSmallUp = { arrowSmallUpArray, 5, 8 };
static const lcd::Image arrowSmallDown = { arrowSmallDownArray, 5, 8 };
static const lcd::Image play = { playArray, 16, 16 };
static const lcd::Image recordingOn = { recordingOnArray, 16, 16 };
static const lcd::Image sessionRecordingOn = { sessionRecordingOnArray, 16, 16 };
static const lcd::Image nudgeDownInactive = { nudgeDownInactiveArray, 10, 8};
static const lcd::Image nudgeDownActive = { nudgeDownActiveArray, 10, 8};
static const lcd::Image nudgeUpInactive = { nudgeUpInactiveArray, 10, 8};
static const lcd::Image nudgeUpActive = { nudgeUpActiveArray, 10, 8};

static const char launchpad95ModeString[8][16] = {
        "Session",
        "Instrument",
        "Device control",
        "User 1",
        "Drum step seq.",
        "Melodic seq.",
        "User 2",
        "Mixer"
};

static const char launchpad95SubmodeString[9][15] = {
        " ",
        "Scale",
        "Volume",
        "Pan",
        "Send A",
        "Send B",
        "MSS: Length",
        "MSS: Octave",
        "MSS: Velocity"
};

static const uint32_t kMidiActivityTimeoutMs = 1000;
static const uint32_t kRotaryControlTimeoutMs = 1000;

LcdGui::LcdGui( Launchpad& launchpad, lcd::Lcd& lcd ):
        launchpad_( launchpad ),
        lcd_( lcd )
{
}

void LcdGui::initialize()
{
    lcd_.clear();
    lcd_.print( "L95", lcd_.horizontalCenter, 0, lcd::Justification_CENTER );
    lcd_.displayImage( 63, 0, usbSymbolSmall );
    lcd_.displayImage( 0, 0, midiSymbolSmall );

    displayLaunchpad95Info();
}

void LcdGui::refresh()
{
    refreshStatusBar();
    refreshMainArea();
}

void LcdGui::refreshStatusBar()
{
    if (midiInputActivityIcon_.isOn)
    {
        if (freertos::Ticks::GetTicks() >= midiInputActivityIcon_.timeToDisable)
        {
            lcd_.clearArea( 73, 0, 77, 7 );
            midiInputActivityIcon_.isOn = false;
        }
    }

    if (midiOutputActivityIcon_.isOn)
    {
        if (freertos::Ticks::GetTicks() >= midiOutputActivityIcon_.timeToDisable)
        {
            lcd_.clearArea( 78, 0, 83, 7 );
            midiOutputActivityIcon_.isOn = false;
        }
    }
}

void LcdGui::refreshMainArea()
{
    if (rotaryControlValues_.isOn)
    {
        if (freertos::Ticks::GetTicks() >= rotaryControlValues_.timeToDisable)
        {
            rotaryControlValues_.isOn = false;
        }
    }

    displayLaunchpad95Info();
}

void LcdGui::registerMidiInputActivity()
{
    lcd_.displayImage( 73, 0, arrowSmallDown );
    midiInputActivityIcon_.isOn = true;
    midiInputActivityIcon_.timeToDisable = freertos::Ticks::GetTicks() + kMidiActivityTimeoutMs;
}


void LcdGui::registerMidiOutputActivity()
{
    lcd_.displayImage( 78, 0, arrowSmallUp );
    midiOutputActivityIcon_.isOn = true;
    midiOutputActivityIcon_.timeToDisable = freertos::Ticks::GetTicks() + kMidiActivityTimeoutMs;
}

void LcdGui::displayLaunchpad95Info()
{
    if (Launchpad95Submode_DEFAULT == launchpad_.submode_)
    {
        displayMode();
    }
    else
    {
        displaySubmode();
    }

    // only display other info when rotary control display timer runs out
    if (!rotaryControlValues_.isOn)
    {
        lcd_.clearArea( 0, 16, 83, 31 );
        displayStatus();

        lcd_.clearArea( 0, 32, 83, 47 );
        switch (launchpad_.mode_)
        {
            case Launchpad95Mode_INSTRUMENT:
            case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            case Launchpad95Mode_MELODIC_SEQUENCER:
                displayTrackName();
                displayClipName();
                break;
            case Launchpad95Mode_DEVICE_CONTROLLER:
                displayTrackName();
                displayDeviceName();
                break;
            case Launchpad95Mode_SESSION:
            case Launchpad95Mode_MIXER:
            case Launchpad95Mode_USER1:
            case Launchpad95Mode_USER2:
            default:
                displayTimingStatus();
                break;
        }
    }
}

void LcdGui::displayClipName()
{
    lcd_.print( launchpad_.clipName_, lcd_.horizontalCenter, 40, lcd::Justification_CENTER );
}

void LcdGui::displayDeviceName()
{
    lcd_.print( launchpad_.deviceName_, lcd_.horizontalCenter, 40, lcd::Justification_CENTER );
}

void LcdGui::displayTrackName()
{
    lcd_.print( launchpad_.trackName_, lcd_.horizontalCenter, 32, lcd::Justification_CENTER );
}

void LcdGui::displayMode()
{
    lcd_.clearArea( 0, 8, 83, 15 );
    if (Launchpad95Mode_UNKNOWN != launchpad_.mode_)
    {
        lcd_.print( launchpad95ModeString[launchpad_.mode_], lcd_.horizontalCenter, 8, lcd::Justification_CENTER );
    }
}

void LcdGui::displaySubmode()
{
    lcd_.clearArea( 0, 8, 83, 15 );
    lcd_.print( launchpad95SubmodeString[launchpad_.submode_], lcd_.horizontalCenter, 8, lcd::Justification_CENTER );
}

void LcdGui::displayStatus()
{
    uint8_t numberOfDisplayedSymbols = (launchpad_.isPlaying_) ? 1 : 0;
    numberOfDisplayedSymbols += (launchpad_.isRecording_) ? 1 : 0;
    numberOfDisplayedSymbols += (launchpad_.isSessionRecording_) ? 1 : 0;

    switch (numberOfDisplayedSymbols)
    {
        case 1:
            lcd_.displayImage( 32, 16, play );
            break;
        case 2:
            lcd_.displayImage( 23, 16, play );
            lcd_.displayImage( 43, 16, (launchpad_.isRecording_ ? recordingOn : sessionRecordingOn) );
            break;
        case 3:
            lcd_.displayImage( 12, 16, play );
            lcd_.displayImage( 32, 16, recordingOn );
            lcd_.displayImage( 52, 16, sessionRecordingOn );
            break;
        default:
            break;
    }
}

void LcdGui::displayTimingStatus()
{
    if (0 != launchpad_.tempo_) // tempo of 0 means there's no info, so no need to display it
    {
        lcd_.displayImage( 0, 40, (launchpad_.nudgeDownActive_ ? nudgeDownActive : nudgeDownInactive) );
        lcd_.displayImage( 10, 40, (launchpad_.nudgeUpActive_ ? nudgeUpActive : nudgeUpInactive) );

        lcd_.printNumberInBigDigits( launchpad_.tempo_, 65, 32, lcd::Justification_RIGHT );
        lcd_.print( "bpm", 66, 32 );

        char signatureString[6];
        std::sprintf( signatureString, "%d/%d", launchpad_.signatureNumerator_, launchpad_.signatureDenominator_ );
        lcd_.print( signatureString, 0, 32 );
    }
}

void LcdGui::displayRotaryControlValues()
{
    char str[4];
    lcd_.clearArea( 0, 16, 83, 47 );

    lcd_.displayProgressArc( 0, 20, (launchpad_.rotaryControlValue_[0] * (lcd_.numberOfProgressArcPositions - 1)) / 127 );
    sprintf( str, "%d", launchpad_.rotaryControlValue_[0] );
    lcd_.print( str, 18, 32, lcd::Justification_CENTER );

    lcd_.displayProgressArc( 45, 20, (launchpad_.rotaryControlValue_[1] * (lcd_.numberOfProgressArcPositions - 1)) / 127 );
    sprintf( str, "%d", launchpad_.rotaryControlValue_[1] );
    lcd_.print( str, 63, 32, lcd::Justification_CENTER );

    rotaryControlValues_.isOn = true;
    rotaryControlValues_.timeToDisable = freertos::Ticks::GetTicks() + kRotaryControlTimeoutMs;
}

} // namespace
}

