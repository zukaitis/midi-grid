#include "lcd/Gui.h"
#include "lcd/Lcd.h"
#include "lcd/images.h"

#include "ticks.hpp"

#include <stdio.h>
#include <string.h>

namespace lcd
{

static const Image usbLogo = { usbLogoArray, 180, 60, 24 };

static const Image usbSymbolSmall = { usbSymbolSmallArray, 9, 9, 8 };
static const Image midiSymbolSmall = { midiSymbolSmallArray, 8, 8, 8 };
static const Image arrowSmallUp = { arrowSmallUpArray, 5, 5, 8 };
static const Image arrowSmallDown = { arrowSmallDownArray, 5, 5, 8 };

static const Image play = { playArray, 32, 16, 16 };
static const Image recordingOn = { recordingOnArray, 32, 16, 16 };
static const Image sessionRecordingOn = { sessionRecordingOnArray, 32, 16, 16 };
static const Image nudgeDownInactive = { nudgeDownInactiveArray, 10, 10, 8};
static const Image nudgeDownActive = { nudgeDownActiveArray, 10, 10, 8};
static const Image nudgeUpInactive = { nudgeUpInactiveArray, 10, 10, 8};
static const Image nudgeUpActive = { nudgeUpActiveArray, 10, 10, 8};

static const uint16_t kTimeoutCheckStep = 250; // ms
static const uint16_t kMidiTimeout = 1250; // ms
static const uint16_t kRotaryControlTimeout = 1250; // ms

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

Gui::Gui( Lcd& lcd ) :
        Thread( "Lcd_Gui", 100, 2 ),
        lcd_( lcd ),
        dawClipName_( " \0" ),
        dawDeviceName_( " \0" ),
        dawTrackName_( " \0" ),
        dawIsPlaying_( false ),
        dawIsRecording_( false ),
        dawIsSessionRecording_( false ),
        dawNudgeDownActive_( false ),
        dawNudgeUpActive_( false ),
        dawTempo_( 0 ),
        dawSignatureNumerator_( 0 ),
        dawSignatureDenominator_( 0 ),
        launchpad95Mode_( launchpad::Launchpad95Mode_UNKNOWN ),
        launchpad95Submode_( launchpad::Launchpad95Submode_DEFAULT ),
        midiInputTimeout_( 0 ),
        midiOutputTimeout_( 0 ),
        rotaryControlDisplayTimeout_( 0 ),
        statusBarActive_( false )
{
    Start();
}

Gui::~Gui()
{}

void Gui::displayConnectingImage()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, usbLogo );
    lcd_.print( "USB Connecting", lcd_.width/2, 40, lcd::Justification_CENTER );
}

void Gui::displayRotaryControlValues( const uint8_t value1, const uint8_t value2 )
{
    char str[4];
    lcd_.clearArea( 0, 16, 83, 47 );

    lcd_.displayProgressArc( 0, 20, (value1 * (lcd_.numberOfProgressArcPositions - 1)) / 127 );
    sprintf( str, "%d", value1 );
    lcd_.print( str, 18, 32, lcd::Justification_CENTER );

    lcd_.displayProgressArc( 45, 20, (value2 * (lcd_.numberOfProgressArcPositions - 1)) / 127 );
    sprintf( str, "%d", value2 );
    lcd_.print( str, 63, 32, lcd::Justification_CENTER );

    rotaryControlDisplayTimeout_ = kRotaryControlTimeout;
}

void Gui::displayWaitingForMidi()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, usbLogo );
    lcd_.print( "Awaiting MIDI", lcd_.width/2, 40, lcd::Justification_CENTER );
}

void Gui::enterLaunchpad95Mode()
{
    lcd_.clear();
    lcd_.print( "L95", lcd_.width/2, 0, lcd::Justification_CENTER );
    lcd_.displayImage( 63, 0, usbSymbolSmall );
    lcd_.displayImage( 0, 0, midiSymbolSmall );

    statusBarActive_ = true;

    displayLaunchpad95Info();
}

void Gui::enterInternalMenu()
{
    lcd_.clear();
    lcd_.print( "Internal Menu", lcd_.width/2, 16, lcd::Justification_CENTER );
    lcd_.print( "Active", lcd_.width/2, 24, lcd::Justification_CENTER );
    statusBarActive_ = false;
}

void Gui::registerMidiInputActivity()
{
    midiInputTimeout_ = kMidiTimeout;
}

void Gui::registerMidiOutputActivity()
{
    midiOutputTimeout_ = kMidiTimeout;
}

void Gui::Run()
{
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( kTimeoutCheckStep );

    while (1)
    {
        DelayUntil( delayPeriod );

        if (statusBarActive_)
        {
            refreshStatusBar();
        }
        refreshMainArea();
    }
}

void Gui::refreshStatusBar()
{
    if (midiInputTimeout_ > 0)
    {
        midiInputTimeout_ -= kTimeoutCheckStep;
        if (midiInputTimeout_ > 0)
        {
            lcd_.displayImage( 73, 0, arrowSmallDown );
        }
        else
        {
            lcd_.clearArea( 73, 0, 77, 7 );
        }
    }

    if (midiOutputTimeout_ > 0)
    {
        midiOutputTimeout_ -= kTimeoutCheckStep;
        if (midiOutputTimeout_ > 0)
        {
            lcd_.displayImage( 78, 0, arrowSmallUp );
        }
        else
        {
            lcd_.clearArea( 78, 0, 83, 7 );
        }
    }
}

void Gui::refreshMainArea()
{
    if (rotaryControlDisplayTimeout_ > 0)
    {
        rotaryControlDisplayTimeout_ -= kTimeoutCheckStep;
        if (0  == rotaryControlDisplayTimeout_)
        {
            // time ran out, back to showing info
            displayLaunchpad95Info();
        }
    }
}


void Gui::setDawClipName( const char* const name, const uint8_t length )
{
    if (length > lcd_.numberOfCharactersInLine)
    {
        strncpy( dawClipName_, &name[0], lcd_.numberOfCharactersInLine );
        dawClipName_[lcd_.numberOfCharactersInLine] = '\0'; // shorten the name
    }
    else
    {
        strcpy( dawClipName_, &name[0] );
    }

    displayLaunchpad95Info();
}

void Gui::setDawDeviceName( const char* const name, const uint8_t length )
{
    if (length > lcd_.numberOfCharactersInLine)
    {
        strncpy( dawDeviceName_, &name[0], lcd_.numberOfCharactersInLine );
        dawDeviceName_[lcd_.numberOfCharactersInLine] = '\0'; // shorten the name
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
    if (length > lcd_.numberOfCharactersInLine)
    {
        strncpy( dawTrackName_, name, lcd_.numberOfCharactersInLine );
        dawTrackName_[lcd_.numberOfCharactersInLine] = '\0'; // shorten the name
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
    lcd_.print( dawClipName_, lcd_.width/2, 40, lcd::Justification_CENTER );
}

void Gui::displayDeviceName()
{
    lcd_.print( dawDeviceName_, lcd_.width/2, 40, lcd::Justification_CENTER );
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
        lcd_.print( launchpad95ModeString[launchpad95Mode_], lcd_.width/2, 8, lcd::Justification_CENTER );
    }
}

void Gui::displayLaunchpad95Submode()
{
    lcd_.clearArea( 0, 8, 83, 15 );
    lcd_.print( launchpad95SubmodeString[launchpad95Submode_], lcd_.width/2, 8, lcd::Justification_CENTER );
}

void Gui::displayStatus()
{
    uint8_t numberOfDisplayedSymbols = (dawIsPlaying_ ? 1 : 0);
    numberOfDisplayedSymbols += (dawIsRecording_ ? 1 : 0);
    numberOfDisplayedSymbols += (dawIsSessionRecording_ ? 1 : 0);

    switch (numberOfDisplayedSymbols)
    {
        case 1:
            lcd_.displayImage( 32, 16, play );
            break;
        case 2:
            lcd_.displayImage( 23, 16, play );
            lcd_.displayImage( 43, 16, (dawIsRecording_ ? recordingOn : sessionRecordingOn) );
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

void Gui::displayTimingStatus()
{
    if (0 != dawTempo_) // tempo of 0 means there's no info, so no need to display it
    {
        char signatureString[6];

        lcd_.displayImage( 0, 40, (dawNudgeDownActive_ ? nudgeDownActive : nudgeDownInactive) );
        lcd_.displayImage( 10, 40, (dawNudgeUpActive_ ? nudgeUpActive : nudgeUpInactive) );

        lcd_.printNumberInBigDigits( dawTempo_, 65, 32, lcd::Justification_RIGHT );
        lcd_.print( "bpm", 66, 32 );

        sprintf( signatureString, "%d/%d", dawSignatureNumerator_, dawSignatureDenominator_ );
        lcd_.print( signatureString, 0, 32 );
    }
}

void Gui::displayTrackName()
{
    lcd_.print( dawTrackName_, lcd_.width/2, 32, lcd::Justification_CENTER );
}

} // namespace lcd
