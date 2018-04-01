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

void Gui::setLaunchpad95Mode( launchpad::Launchpad95Mode mode )
{
    launchpad95Mode = mode;
    displayLaunchpad95Info();
}

void Gui::displayLaunchpad95Mode()
{
    lcd.clearArea(0, 8, 83, 15);
    if (launchpad::Launchpad95Mode_UNKNOWN != launchpad95Mode)
    {
        lcd.print(launchpad95ModeString[launchpad95Mode], lcd::WIDTH/2, 8, lcd::Justification_CENTER);
    }
}

void Gui::setLaunchpad95Submode( launchpad::Launchpad95Submode submode )
{
    launchpad95Submode = submode;
    displayLaunchpad95Info();
}

void Gui::displayLaunchpad95Submode()
{
    lcd.clearArea(0, 8, 83, 15);
    lcd.print(launchpad95SubmodeString[launchpad95Submode], lcd::WIDTH/2, 8, lcd::Justification_CENTER);
}

void Gui::setTrackName(char* name, uint8_t length)
{
    if (length > 14)
    {
        strncpy(trackName, name, 14);
        trackName[14] = '\0'; // shorten the name
    }
    else
    {
        strcpy(trackName, name);
    }

    displayLaunchpad95Info();
}

void Gui::displayTrackName()
{
    lcd.print(trackName, lcd::WIDTH/2, 32, lcd::Justification_CENTER);
}

void Gui::setClipName(char* name, uint8_t length)
{
    if (length > 14)
    {
        strncpy(clipName, name, 14);
        clipName[14] = '\0'; // shorten the name
    }
    else
    {
        strcpy(clipName, name);
    }

    displayLaunchpad95Info();
}

void Gui::displayClipName()
{
    lcd.print(clipName, lcd::WIDTH/2, 40, lcd::Justification_CENTER);
}

void Gui::setDeviceName(char* name, uint8_t length)
{
    if (length > 14)
    {
        strncpy(deviceName, name, 14);
        deviceName[14] = '\0'; // shorten the name
    }
    else
    {
        strcpy(deviceName, name);
    }

    displayLaunchpad95Info();
}

void Gui::setStatus(const bool isPlaying, const bool isRecording, const bool isSessionRecording)
{
    isPlaying_ = isPlaying;
    isRecording_ = isRecording;
    isSessionRecording_ = isSessionRecording;

    displayLaunchpad95Info();
}

void Gui::setTimingValues( const uint16_t tempo, const uint8_t signatureNumerator, const uint8_t signatureDenominator,
        const bool nudgeDown, const bool nudgeUp )
{
    tempo_ = tempo;
    signatureNumerator_ = signatureNumerator;
    signatureDenominator_ = signatureDenominator;
    nudgeDownActive_ = nudgeDown;
    nudgeUpActive_ = nudgeUp;

    displayLaunchpad95Info();
}

void Gui::displayDeviceName()
{
    lcd.print(deviceName, lcd::WIDTH/2, 40, lcd::Justification_CENTER);
}

void Gui::displayUsbLogo()
{
    lcd.displayImage(0, 0, lcd::usbLogo);
}

void Gui::displayStatusBar()
{
    lcd.clearArea(0, 0, 83, 7);
    lcd.print( "L95", lcd::WIDTH/2, 0, lcd::Justification_CENTER );
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

void Gui::displayLaunchpad95Info()
{
    if (launchpad::Launchpad95Submode_DEFAULT == launchpad95Submode)
    {
        displayLaunchpad95Mode();
    }
    else
    {
        displayLaunchpad95Submode();
    }

    lcd.clearArea(0, 16, 83, 31);
    displayStatus();

    lcd.clearArea(0, 32, 83, 47);
    switch (launchpad95Mode)
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

void Gui::displayStatus()
{
    uint8_t numberOfDisplayedSymbols = (isPlaying_ ? 1 : 0);
    numberOfDisplayedSymbols += (isRecording_ ? 1 : 0);
    numberOfDisplayedSymbols += (isSessionRecording_ ? 1 : 0);

    switch (numberOfDisplayedSymbols)
    {
        case 1:
            lcd.displayImage(32, 16, lcd::play);
            break;
        case 2:
            lcd.displayImage(23, 16, lcd::play);
            lcd.displayImage(43, 16, (isRecording_ ? lcd::recordingOn : lcd::sessionRecordingOn));
            break;
        case 3:
            lcd.displayImage(12, 16, lcd::play);
            lcd.displayImage(32, 16, lcd::recordingOn);
            lcd.displayImage(52, 16, lcd::sessionRecordingOn);
            break;
        default:
            break;
    }
}

void Gui::displayTimingStatus()
{
    char signatureString[6], tempoString[7];
    sprintf(signatureString, "%d/%d", signatureNumerator_, signatureDenominator_);
    sprintf(tempoString, "%dbpm", tempo_);
    lcd.print(signatureString, 0, 32);
    lcd.print(tempoString, 42, 32);

    lcd.displayImage(0, 40, (nudgeDownActive_ ? lcd::nudgeDownActive : lcd::nudgeDownInactive));
    lcd.displayImage(10, 40, (nudgeUpActive_ ? lcd::nudgeUpActive : lcd::nudgeUpInactive));
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

} // namespace
