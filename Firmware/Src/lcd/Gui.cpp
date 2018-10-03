#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "system/Time.h"

namespace lcd
{
namespace gui
{

Gui::Gui( Lcd& lcd, Time& time ) :
        lcd_( lcd ),
        time_( time )
{}

Gui::~Gui()
{}

void Gui::displayConnectingImage()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, lcd::usbLogo );
    lcd_.print( "USB Connecting", lcd::WIDTH/2, 40, lcd::Justification_CENTER );
}

void Gui::displayRotaryControlValues( const uint8_t value1, const uint8_t value2 )
{
    char str[4];
    lcd_.clearArea( 0, 16, 83, 47 );

    lcd_.displayProgressArc( 0, 20, (value1 * (lcd::NUMBER_OF_PROGRESS_ARC_POSITIONS - 1)) / 127 );
    sprintf( str, "%d", value1 );
    lcd_.print( str, 18, 32, lcd::Justification_CENTER );

    lcd_.displayProgressArc( 45, 20, (value2 * (lcd::NUMBER_OF_PROGRESS_ARC_POSITIONS - 1)) / 127 );
    sprintf( str, "%d", value2 );
    lcd_.print( str, 63, 32, lcd::Justification_CENTER );

    rotaryControlDisplayTimeout_ = ROTARY_CONTROL_TIMEOUT;
}

void Gui::displayWaitingForMidi()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, lcd::usbLogo );
    lcd_.print( "Awaiting MIDI", lcd::WIDTH/2, 40, lcd::Justification_CENTER );
}

void Gui::enterLaunchpad95Mode()
{
    lcd_.clear();
    lcd_.print( "L95", lcd::WIDTH/2, 0, lcd::Justification_CENTER );
    lcd_.displayImage( 63, 0, lcd::usbSymbolSmall );

    statusBarActive_ = true;

    displayLaunchpad95Info();
}

void Gui::enterInternalMenu()
{
    lcd_.clear();
    lcd_.print( "Internal Menu", lcd::WIDTH/2, 16, lcd::Justification_CENTER );
    lcd_.print( "Active", lcd::WIDTH/2, 24, lcd::Justification_CENTER );
    statusBarActive_ = false;
}

void Gui::registerMidiInputActivity()
{
    midiInputTimeout_ = MIDI_TIMEOUT;
}

void Gui::registerMidiOutputActivity()
{
    midiOutputTimeout_ = MIDI_TIMEOUT;
}

void Gui::refresh()
{
    if (statusBarActive_)
    {
        refreshStatusBar();
    }

    refreshMainArea();
    lcd_.refresh();
}

void Gui::refreshStatusBar()
{
    static uint32_t refreshCheckTime = 0;

    if (time_.getSystemTick() >= refreshCheckTime)
    {
        if (midiInputTimeout_ > 0)
        {
            midiInputTimeout_ -= TIMEOUT_CHECK_STEP;
            if (midiInputTimeout_ > 0)
            {
                lcd_.displayImage( 73, 0, lcd::arrowSmallDown );
            }
            else
            {
                lcd_.clearArea( 73, 0, 77, 7 );
            }
        }

        if (midiOutputTimeout_ > 0)
        {
            midiOutputTimeout_ -= TIMEOUT_CHECK_STEP;
            if (midiOutputTimeout_ > 0)
            {
                lcd_.displayImage( 78, 0, lcd::arrowSmallUp );
            }
            else
            {
                lcd_.clearArea( 78, 0, 83, 7 );
            }
        }

        refreshCheckTime = time_.getSystemTick() + TIMEOUT_CHECK_STEP; // check every 250ms
    }
}

void Gui::refreshMainArea()
{
    static uint32_t checkTime = 0;

    if (time_.getSystemTick() >= checkTime)
    {
        if (rotaryControlDisplayTimeout_ > 0)
        {
            rotaryControlDisplayTimeout_ -= TIMEOUT_CHECK_STEP;
            if (0  == rotaryControlDisplayTimeout_)
            {
                // time ran out, back to showing info
                displayLaunchpad95Info();
            }
        }

        checkTime = time_.getSystemTick() + TIMEOUT_CHECK_STEP; // check every 250ms
    }
}


void Gui::setDawClipName( const char* const name, const uint8_t length )
{
    if (length > lcd::NUMBER_OF_CHARACTERS_IN_LINE)
    {
        strncpy( dawClipName_, &name[0], lcd::NUMBER_OF_CHARACTERS_IN_LINE );
        dawClipName_[lcd::NUMBER_OF_CHARACTERS_IN_LINE] = '\0'; // shorten the name
    }
    else
    {
        strcpy( dawClipName_, &name[0] );
    }

    displayLaunchpad95Info();
}

void Gui::setDawDeviceName( const char* const name, const uint8_t length )
{
    if (length > lcd::NUMBER_OF_CHARACTERS_IN_LINE)
    {
        strncpy( dawDeviceName_, &name[0], lcd::NUMBER_OF_CHARACTERS_IN_LINE );
        dawDeviceName_[lcd::NUMBER_OF_CHARACTERS_IN_LINE] = '\0'; // shorten the name
    }
    else
    {
        strcpy( dawDeviceName_, &name[0] );
    }

    displayLaunchpad95Info();
}

void Gui::setDawStatus( const bool isPlaying, const bool isRecording, const bool isSessionRecording )
{
    dawIsPlaying_ = isPlaying;
    dawIsRecording_ = isRecording;
    dawIsSessionRecording_ = isSessionRecording;

    displayLaunchpad95Info();
}

void Gui::setDawTimingValues( const uint16_t tempo, const uint8_t signatureNumerator, const uint8_t signatureDenominator,
        const bool nudgeDown, const bool nudgeUp )
{
    dawTempo_ = tempo;
    dawSignatureNumerator_ = signatureNumerator;
    dawSignatureDenominator_ = signatureDenominator;
    dawNudgeDownActive_ = nudgeDown;
    dawNudgeUpActive_ = nudgeUp;

    displayLaunchpad95Info();
}

void Gui::setDawTrackName( const char* const name, const uint8_t length )
{
    if (length > lcd::NUMBER_OF_CHARACTERS_IN_LINE)
    {
        strncpy( dawTrackName_, name, lcd::NUMBER_OF_CHARACTERS_IN_LINE );
        dawTrackName_[lcd::NUMBER_OF_CHARACTERS_IN_LINE] = '\0'; // shorten the name
    }
    else
    {
        strcpy( dawTrackName_, name );
    }

    displayLaunchpad95Info();
}

void Gui::setLaunchpad95Mode( const launchpad::Launchpad95Mode mode )
{
    launchpad95Mode_ = mode;
    displayLaunchpad95Info();
}

void Gui::setLaunchpad95Submode( const launchpad::Launchpad95Submode submode )
{
    launchpad95Submode_ = submode;
    displayLaunchpad95Info();
}

void Gui::displayClipName()
{
    lcd_.print( dawClipName_, lcd::WIDTH/2, 40, lcd::Justification_CENTER );
}

void Gui::displayDeviceName()
{
    lcd_.print( dawDeviceName_, lcd::WIDTH/2, 40, lcd::Justification_CENTER );
}

void Gui::displayLaunchpad95Info()
{
    if (launchpad::Launchpad95Submode_DEFAULT == launchpad95Submode_)
    {
        displayLaunchpad95Mode();
    }
    else
    {
        displayLaunchpad95Submode();
    }

    // only display other info when rotary control display timer runs out
    if (0 == rotaryControlDisplayTimeout_)
    {
        lcd_.clearArea( 0, 16, 83, 31 );
        displayStatus();

        lcd_.clearArea( 0, 32, 83, 47 );
        switch (launchpad95Mode_)
        {
            case launchpad::Launchpad95Mode_INSTRUMENT:
            case launchpad::Launchpad95Mode_DRUM_STEP_SEQUENCER:
            case launchpad::Launchpad95Mode_MELODIC_SEQUENCER:
                displayTrackName();
                displayClipName();
                break;
            case launchpad::Launchpad95Mode_DEVICE_CONTROLLER:
                displayTrackName();
                displayDeviceName();
                break;
            case launchpad::Launchpad95Mode_SESSION:
            case launchpad::Launchpad95Mode_MIXER:
            case launchpad::Launchpad95Mode_USER1:
            case launchpad::Launchpad95Mode_USER2:
            default:
                displayTimingStatus();
                break;
        }
    }
}

void Gui::displayLaunchpad95Mode()
{
    lcd_.clearArea( 0, 8, 83, 15 );
    if (launchpad::Launchpad95Mode_UNKNOWN != launchpad95Mode_)
    {
        lcd_.print( launchpad95ModeString[launchpad95Mode_], lcd::WIDTH/2, 8, lcd::Justification_CENTER );
    }
}

void Gui::displayLaunchpad95Submode()
{
    lcd_.clearArea( 0, 8, 83, 15 );
    lcd_.print( launchpad95SubmodeString[launchpad95Submode_], lcd::WIDTH/2, 8, lcd::Justification_CENTER );
}

void Gui::displayStatus()
{
    uint8_t numberOfDisplayedSymbols = (dawIsPlaying_ ? 1 : 0);
    numberOfDisplayedSymbols += (dawIsRecording_ ? 1 : 0);
    numberOfDisplayedSymbols += (dawIsSessionRecording_ ? 1 : 0);

    switch (numberOfDisplayedSymbols)
    {
        case 1:
            lcd_.displayImage( 32, 16, lcd::play );
            break;
        case 2:
            lcd_.displayImage( 23, 16, lcd::play );
            lcd_.displayImage( 43, 16, (dawIsRecording_ ? lcd::recordingOn : lcd::sessionRecordingOn) );
            break;
        case 3:
            lcd_.displayImage( 12, 16, lcd::play );
            lcd_.displayImage( 32, 16, lcd::recordingOn );
            lcd_.displayImage( 52, 16, lcd::sessionRecordingOn );
            break;
        default:
            break;
    }
}

void Gui::displayTimingStatus()
{
    if (0 != dawTempo_) // tempo of 0 means there's no info, so no need to display it
    {
        char signatureString[6];

        lcd_.displayImage( 0, 40, (dawNudgeDownActive_ ? lcd::nudgeDownActive : lcd::nudgeDownInactive) );
        lcd_.displayImage( 10, 40, (dawNudgeUpActive_ ? lcd::nudgeUpActive : lcd::nudgeUpInactive) );

        lcd_.printNumberInBigDigits( dawTempo_, 65, 32, lcd::Justification_RIGHT );
        lcd_.print( "bpm", 66, 32 );

        sprintf( signatureString, "%d/%d", dawSignatureNumerator_, dawSignatureDenominator_ );
        lcd_.print( signatureString, 0, 32 );
    }
}

void Gui::displayTrackName()
{
    lcd_.print( dawTrackName_, lcd::WIDTH/2, 32, lcd::Justification_CENTER );
}

} // namespace gui
} // namespace lcd
