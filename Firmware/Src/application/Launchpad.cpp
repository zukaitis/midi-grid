#include "application/Launchpad.h"

#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"

#include "usb/usb_device.h"
#include "usb/queue32.h"

namespace launchpad
{

/* MIDI */
static const uint8_t kControlValueLow = 0;
static const uint8_t kControlValueHigh = 127;

static const uint8_t kChallengeResponseLength = 10;
static const uint8_t kChallengeResponse[kChallengeResponseLength] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7 };

static const uint8_t kStandardSystemExclussiveMessageHeaderLength = 6;
static const uint8_t kStandardSystemExclussiveMessageHeader[kStandardSystemExclussiveMessageHeaderLength] =
        { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18 };
static const uint8_t kStandardSystemExclussiveMessageMinimumLength = 8;

enum StandardSystemExclussiveMessageType
{
    kTextScroll = 0x14,
    kSetLayout = 0x22,
    kChallenge = 0x40
};

enum MidiChannel
{
    kSessionLayoutMidiChannel = 0,
    kDeviceControlMidiChannel = 0,
    kAdditionalControlMidiChannel = 5,
    kUser1LayoutMidiChannel = 7, // can select channel from 6, 7 and 8
    kUser2LayoutMidiChannel = 15 // can select channel from 14, 15 and 16
};

static const uint8_t kAdditionalNoteButtonNote = 55;

/* Grid */
static const uint8_t kNumberOfColumns = 10;
static const uint8_t kNumberOfRows = 8;

static const uint8_t kSubmodeColumn = 8;
static const uint8_t kDeviceControlColumn = 9;

struct DeviceControlButton
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t controlValue;
};

static const DeviceControlButton kSession = {.positionX = 9, .positionY = 3, .controlValue = 108};
static const DeviceControlButton kMixer = {.positionX = 9, .positionY = 1, .controlValue = 111};
static const DeviceControlButton kUser1 = {.positionX = 9, .positionY = 2, .controlValue = 109};
static const DeviceControlButton kUser2 = {.positionX = 9, .positionY = 0, .controlValue = 110};
static const DeviceControlButton kUp = {.positionX = 9, .positionY = 4, .controlValue = 104};
static const DeviceControlButton kDown = {.positionX = 9, .positionY = 7, .controlValue = 105};
static const DeviceControlButton kLeft = {.positionX = 9, .positionY = 5, .controlValue = 106};
static const DeviceControlButton kRight = {.positionX = 9, .positionY = 6, .controlValue = 107};

static const uint8_t kMinimumDeviceControlValue = kUp.controlValue;
static const uint8_t kMaximumDeviceControlValue = kMixer.controlValue;

// buttons are numerated bottom to top, same as in grid
static const uint8_t kDeviceControlColumnValue[kNumberOfRows] = {
        kUser2.controlValue, kMixer.controlValue, kUser1.controlValue, kSession.controlValue,
        kUp.controlValue, kLeft.controlValue, kRight.controlValue, kDown.controlValue };

static const uint8_t kSessionLayout[kNumberOfColumns][kNumberOfRows] = {
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t kDrumLayout[kNumberOfColumns][kNumberOfRows] = {
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t kLaunchpadColorPaletteSize = 128;
static const Color kLaunchpadColorPalette[kLaunchpadColorPaletteSize] = {
        {0, 0, 0}, {8, 8, 8}, {32, 32, 32}, {64, 64, 64}, {64, 20, 18}, {64, 3, 0}, {23, 1, 0}, {7, 0, 0},
        {64, 48, 25}, {64, 22, 0}, {23, 8, 0}, {10, 7, 0}, {64, 64, 9}, {64, 64, 0}, {23, 23, 0}, {6, 6, 0},
        {33, 64, 11}, {17, 64, 0}, {6, 23, 0}, {5, 11, 0}, {14, 64, 11}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0},
        {13, 64, 18}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0}, {13, 64, 32}, {0, 64, 15}, {0, 23, 6}, {0, 8, 4},
        {12, 64, 45}, {0, 64, 37}, {0, 23, 13}, {0, 7, 4}, {15, 48, 64}, {0, 42, 64}, {0, 17, 21}, {0, 5, 7},
        {17, 34, 64}, {0, 21, 64}, {0, 7, 23}, {0, 2, 7}, {18, 18, 64}, {0, 0, 64}, {0, 0, 23}, {0, 0, 7},
        {33, 18, 64}, {21, 0, 64}, {6, 0, 26}, {3, 0, 13}, {64, 19, 64}, {64, 0, 64}, {23, 0, 23}, {7, 0, 7},
        {64, 20, 34}, {64, 2, 21}, {23, 1, 7}, {9, 0, 5}, {64, 7, 0}, {39, 14, 0}, {31, 21, 0}, {16, 26, 0},
        {0, 15, 0}, {0, 22, 13}, {0, 21, 32}, {0, 0, 64}, {0, 18, 20}, {7, 0, 53}, {32, 32, 32}, {9, 9, 9},
        {64, 3, 0}, {47, 64, 0}, {43, 60, 0}, {22, 64, 0}, {0, 35, 0}, {0, 64, 31}, {0, 42, 64}, {0, 7, 64},
        {14, 0, 64}, {30, 0, 64}, {46, 6, 32}, {17, 9, 0}, {64, 19, 0}, {33, 57, 0}, {26, 64, 0}, {0, 64, 0},
        {0, 64, 0}, {18, 64, 25}, {0, 64, 51}, {21, 34, 64}, {10, 20, 51}, {33, 31, 60}, {53, 4, 64}, {64, 2, 23},
        {64, 32, 0}, {47, 45, 0}, {35, 64, 0}, {33, 24, 0}, {14, 11, 0}, {4, 20, 2}, {0, 21, 14}, {5, 5, 11},
        {5, 8, 23}, {27, 16, 6}, {44, 2, 0}, {64, 21, 14}, {56, 27, 0}, {64, 64, 0}, {39, 57, 0}, {24, 46, 0},
        {7, 7, 13}, {56, 64, 21}, {30, 64, 47}, {38, 38, 64}, {35, 25, 64}, {17, 17, 17}, {30, 30, 30}, {56, 64, 64},
        {42, 2, 0}, {14, 0, 0}, {0, 53, 0}, {0, 17, 0}, {47, 45, 0}, {16, 13, 0}, {46, 24, 0}, {19, 6, 0} };

Launchpad::Launchpad( grid::Grid& grid, grid::Switches& switches, lcd::Gui& gui, midi::UsbMidi& usbMidi ) :
        grid_( grid ),
        switches_( switches ),
        gui_( gui ),
        usbMidi_( usbMidi ),
        currentLaunchpad95Mode_( Launchpad95Mode_UNKNOWN ),
        currentLayout_( Layout_SESSION ),
        systemExclusiveInputMessageLength_( 0 )
{
    const int16_t initialControlValue = midi::kMaximumControlValue / 2;
    rotaryControlValue_[0] = initialControlValue;
    rotaryControlValue_[1] = initialControlValue;
}

void Launchpad::runProgram()
{
    grid_.discardAllPendingButtonEvents();
    grid_.turnAllLedsOff();
    switches_.discardAllPendingEvents();

    sendMixerModeControlMessage();

    gui_.enterLaunchpad95Mode();

    bool stopApplication = false;

    while (!stopApplication)
    {
        stopApplication |= handleMidiInput();
        stopApplication |= handleGridInput();
        stopApplication |= handleAdditionalControlInput();
    }
}

Launchpad95Mode Launchpad::determineLaunchpad95Mode()
{
    Launchpad95Mode mode = Launchpad95Mode_UNKNOWN;

    do
    {
        Color color = grid_.getLedColor( kDeviceControlColumn, kSession.positionY );
        if (grid_.areColorsEqual( color, kLaunchpadColorPalette[21] ))
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kUser1.positionY );
        if (grid_.areColorsEqual( color, kLaunchpadColorPalette[37] ))
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        else if (grid_.areColorsEqual( color, kLaunchpadColorPalette[48] ))
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        else if (grid_.areColorsEqual( color, kLaunchpadColorPalette[45] ))
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kUser2.positionY );
        if (grid_.areColorsEqual( color, kLaunchpadColorPalette[53] ))
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        else if (grid_.areColorsEqual( color, kLaunchpadColorPalette[9] ))
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        else if (grid_.areColorsEqual( color, kLaunchpadColorPalette[45] ))
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kMixer.positionY );
        if (grid_.areColorsEqual( color, kLaunchpadColorPalette[29] ))
        {
            mode = Launchpad95Mode_MIXER;
            break;
        }
    } while (false);

    return mode;
}

Launchpad95Submode Launchpad::determineLaunchpad95Submode()
{
    Launchpad95Submode submode = Launchpad95Submode_DEFAULT;
    Color color;

    switch (currentLaunchpad95Mode_)
    {
        case Launchpad95Mode_INSTRUMENT:
            color = grid_.getLedColor( kSubmodeColumn, 7 );
            if (grid_.areColorsEqual( color, kLaunchpadColorPalette[9] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            color = grid_.getLedColor( kSubmodeColumn, 7 );
            if (grid_.areColorsEqual( color, kLaunchpadColorPalette[5] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_MIXER:
            do
            {
                color = grid_.getLedColor( kSubmodeColumn, 7 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_VOLUME;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 6 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_PAN;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 5 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_A;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 4 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_B;
                }
            } while (false);
            break;
        case Launchpad95Mode_MELODIC_SEQUENCER:
            do
            {
                color = grid_.getLedColor( kSubmodeColumn, 3 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[29] ))
                {
                    submode = Launchpad95Submode_LENGTH;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 2 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[48] ))
                {
                    submode = Launchpad95Submode_OCTAVE;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 1 );
                if (grid_.areColorsEqual( color, kLaunchpadColorPalette[37] ))
                {
                    submode = Launchpad95Submode_VELOCITY;
                }
            } while (false);
            break;
        default:
            break;
    }

    return submode;
}

bool Launchpad::handleAdditionalControlInput()
{
    bool stopApplication = false;

    ButtonAction event;
    uint8_t encoderNumber;
    int8_t rotaryStep;

    if (switches_.getRotaryEncoderEvent( encoderNumber, rotaryStep ))
    {
        rotaryControlValue_[encoderNumber] += rotaryStep;
        if (rotaryControlValue_[encoderNumber] > midi::kMaximumControlValue)
        {
            rotaryControlValue_[encoderNumber] = midi::kMaximumControlValue;
        }
        else if (rotaryControlValue_[encoderNumber] < midi::kMinimumControlValue)
        {
            rotaryControlValue_[encoderNumber] = midi::kMinimumControlValue;
        }
        usbMidi_.sendControlChange( kAdditionalControlMidiChannel, encoderNumber, rotaryControlValue_[encoderNumber] );
        gui_.registerMidiOutputActivity();
        gui_.displayRotaryControlValues( static_cast<uint8_t>(rotaryControlValue_[0]), static_cast<uint8_t>(rotaryControlValue_[1]) );
    }

    uint8_t button;

    if (switches_.getButtonEvent( button,  event ))
    {
        if (switches_.additionalNoteButton == button) // only send note on the event of black button
        {
            const uint8_t controlValue = (ButtonAction_PRESSED == event) ? kControlValueHigh : kControlValueLow;
            usbMidi_.sendNoteOn( kAdditionalControlMidiChannel, kAdditionalNoteButtonNote, controlValue );
            gui_.registerMidiOutputActivity();
        }
        else if (switches_.internalMenuButton == button)
        {
            if (ButtonAction_PRESSED == event)
            {
                stopApplication = true; // break launchpad mode loop, enter internal menu
            }
        }
    }

    return stopApplication;
}

bool Launchpad::handleMidiInput()
{
    const bool stopApplication = false;

    midi::MidiPacket inputPacket;

    if (usbMidi_.getPacket( inputPacket ))
    {
        const uint8_t codeIndexNumber = inputPacket.header & midi::kCodeIndexNumberMask;
        switch (codeIndexNumber)
        {
            case midi::kNoteOn:
                processNoteOnMidiMessage( inputPacket.data[0] & midi::kChannelMask, inputPacket.data[1], inputPacket.data[2] );
                break;
            case midi::kNoteOff:
                processNoteOnMidiMessage( inputPacket.data[0] & midi::kChannelMask, inputPacket.data[1], 0 );
                break;
            case midi::kControlChange:
                processChangeControlMidiMessage( inputPacket.data[0] & midi::kChannelMask, inputPacket.data[1], inputPacket.data[2] );
                break;
            case midi::kSystemExclusive:
            case midi::kSystemExclusiveEnd1Byte:
            case midi::kSystemExclusiveEnd2Bytes:
            case midi::kSystemExclusiveEnd3Bytes:
                processSystemExclusiveMidiPacket( inputPacket );
                break;
            default:
                break;
        }
        gui_.registerMidiInputActivity();
    }

    return stopApplication;
}

bool Launchpad::handleGridInput()
{
    const bool stopApplication = false;

    uint8_t buttonX, buttonY;
    ButtonAction event;

    if (grid_.getButtonEvent( buttonX, buttonY, event ))
    {
        const uint8_t controlValue = (ButtonAction_PRESSED == event) ? kControlValueHigh : kControlValueLow;
        if (kDeviceControlColumn == buttonX) // control column
        {
            usbMidi_.sendControlChange( kDeviceControlMidiChannel, kDeviceControlColumnValue[buttonY], controlValue );
        }
        else
        {
            switch (currentLayout_)
            {
                case Layout_USER1:
                    usbMidi_.sendNoteOn( kUser1LayoutMidiChannel, kDrumLayout[buttonX][buttonY], controlValue );
                    break;
                case Layout_USER2:
                    usbMidi_.sendNoteOn( kUser2LayoutMidiChannel, kSessionLayout[buttonX][buttonY], controlValue );
                    break;
                case Layout_SESSION:
                default:
                    usbMidi_.sendNoteOn( kSessionLayoutMidiChannel, kSessionLayout[buttonX][buttonY], controlValue );
                    break;
            }
        }
        gui_.registerMidiOutputActivity();
    }

    return stopApplication;
}

void Launchpad::processDawInfoMessage( const char* const message, uint8_t length )
{
    switch (message[0])
    {
        case 't':
            gui_.setDawTrackName( &message[1], length - 1 );
            break;
        case 'c':
            gui_.setDawClipName( &message[1], length - 1 );
            break;
        case 'd':
            gui_.setDawDeviceName( &message[1], length - 1 );
            break;
        case 's':
            {
                const bool isPlaying = ('P' == message[1]);
                const bool isRecording = ('R' == message[2]);
                const bool isSessionRecording = ('S' == message[3]);
                gui_.setDawStatus( isPlaying, isRecording, isSessionRecording );
            }
            break;
        case 'T':
            {
                const uint16_t tempo = (message[1] - '0')*100 + (message[2] - '0')*10 + (message[3] - '0');
                const uint8_t signatureNumerator = (message[4] - '0')*10 + (message[5] - '0');
                const uint8_t signatureDenominator = (message[6] - '0')*10 + (message[7] - '0');
                const bool nudgeDownActive = ('D' == message[8]);
                const bool nudgeUpActive = ('U' == message[8]);
                gui_.setDawTimingValues( tempo, signatureNumerator, signatureDenominator, nudgeDownActive, nudgeUpActive );
            }
            break;
        default:
            break;
    }
}

void Launchpad::processChangeControlMidiMessage( const uint8_t channel, const uint8_t control, const uint8_t value )
{
    if ((control >= kMinimumDeviceControlValue) && (control <= kMaximumDeviceControlValue))
    {
        const uint8_t ledPositionX = kDeviceControlColumn;

        for (uint8_t y = 0; y < grid::numberOfRows; y++)
        {
            // find corresponding led
            if (kDeviceControlColumnValue[y] == control)
            {
                const uint8_t ledPositionY = y;
                grid_.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[value], static_cast<grid::LedLightingType>(channel) );
                break;
            }
        }

        if ( (control == kSession.controlValue) ||
            (control == kUser1.controlValue) ||
            (control == kMixer.controlValue) ||
            (control == kUser2.controlValue) )
        {
            currentLaunchpad95Mode_ = determineLaunchpad95Mode();
            gui_.setLaunchpad95Mode( currentLaunchpad95Mode_ );
            if (Launchpad95Mode_MELODIC_SEQUENCER == currentLaunchpad95Mode_)
            {
                // only melodic step sequencer can stay in submode between mode changes
                gui_.setLaunchpad95Submode( determineLaunchpad95Submode() );
            }
            else
            {
                gui_.setLaunchpad95Submode( Launchpad95Submode_DEFAULT );
            }
        }
    }
}

void Launchpad::processNoteOnMidiMessage( uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    uint8_t ledPositionX, ledPositionY;
    bool ledPositionCorrect = false;

    if (Layout_USER1 == currentLayout_)
    {
        // only this layout uses drum layout
        if (note >= kDrumLayout[0][0])
        {
            if (note <= kDrumLayout[3][7])
            {
                ledPositionX = note % 4;
                ledPositionY = (note - kDrumLayout[0][0]) / 4;
                ledPositionCorrect = true;
            }
            else if (note <= kDrumLayout[7][7])
            {
                ledPositionX = (note % 4) + 4;
                ledPositionY = (note - kDrumLayout[4][0]) / 4;
                ledPositionCorrect = true;
            }
            else if (note <= kDrumLayout[8][0])
            {
                ledPositionX = 8;
                ledPositionY = kDrumLayout[8][0] - note;
                ledPositionCorrect = true;
            }
        }
    }
    else
    {
        if ((note >= kSessionLayout[0][0]) && (note <= kSessionLayout[8][7]))
        {
            ledPositionX = (note % 10) - 1;
            ledPositionY = (note / 10) - 1;
            ledPositionCorrect = true;
        }
    }

    if (ledPositionCorrect)
    {
        if (velocity < kLaunchpadColorPaletteSize)
        {
            grid::LedLightingType lightingType = grid::LedLightingType_LIGHT;
            if (channel <= static_cast<uint8_t>(grid::LedLightingType_PULSE))
            {
                lightingType = static_cast<grid::LedLightingType>(channel);
            }

            grid_.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[velocity], lightingType );

            if ((Layout_USER1 != currentLayout_) && (kSubmodeColumn == ledPositionX))
            {
                // possible change in submode
                gui_.setLaunchpad95Submode( determineLaunchpad95Submode() );
            }
        }
    }
}

void Launchpad::processSystemExclusiveMessage( uint8_t* const message, uint8_t length )
{
    if (length >= kStandardSystemExclussiveMessageMinimumLength)
    {
        if (0 == memcmp( message, kStandardSystemExclussiveMessageHeader, kStandardSystemExclussiveMessageHeaderLength ))
        {
            const uint8_t standardMessageType = message[6];
            switch(standardMessageType)
            {
                case kSetLayout:
                    {
                        const uint8_t layoutIndex = message[7];
                        if (layoutIndex <= kMaximumLayoutIndex)
                        {
                            setCurrentLayout( static_cast<Layout>(layoutIndex) );
                        }
                    }
                    break;
                case kChallenge:
                    usbMidi_.sendSystemExclussive( &kChallengeResponse[0], kChallengeResponseLength );
                    gui_.registerMidiOutputActivity();
                    break;
                case kTextScroll:
                    message[length-1] = 0; // put string terminator at the end
                    processDawInfoMessage( reinterpret_cast<char*>(&message[7]), length - 7 - 1 );
                    break;
                default:
                    break;
            }
        }
    }
}

void Launchpad::processSystemExclusiveMidiPacket( const midi::MidiPacket& packet )
{
    const uint8_t codeIndexNumber = packet.header & midi::kCodeIndexNumberMask;

    if (systemExclusiveInputMessageLength_ >= (kSystemExclussiveMessageMaximumLength_ - 3))
    {
        systemExclusiveInputMessageLength_ = 0; // discard this message, as it is too long
    }
    else
    {
        switch (codeIndexNumber)
        {
            case midi::kSystemExclusive:
            case midi::kSystemExclusiveEnd3Bytes:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[1];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[2];
                break;
            case midi::kSystemExclusiveEnd1Byte:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                break;
            case midi::kSystemExclusiveEnd2Bytes:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[1];
                break;
            default:
                break;
        }

        if ((midi::kSystemExclusiveEnd1Byte == codeIndexNumber) || (midi::kSystemExclusiveEnd2Bytes == codeIndexNumber) ||
                (midi::kSystemExclusiveEnd3Bytes == codeIndexNumber))
        {
            processSystemExclusiveMessage( &systemExclusiveInputMessage_[0], systemExclusiveInputMessageLength_ );
            systemExclusiveInputMessageLength_ = 0; // reset message length
        }
    }
}

void Launchpad::sendMixerModeControlMessage()
{
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueHigh );
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueLow );
    gui_.registerMidiOutputActivity();
}

void Launchpad::setCurrentLayout( const Layout layout )
{
    currentLayout_ = layout;
}

} // namespace
