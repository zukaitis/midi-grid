#include "application/launchpad/Launchpad.hpp"

#include "io/grid/Grid.hpp"
#include "io/AdditionalButtons.hpp"
#include "io/RotaryControls.hpp"
#include "io/lcd/Lcd.hpp"

#include <cstring>
#include <etl/array.h>

namespace application
{
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

static const etl::array<Color, 128> kLaunchpadColorPalette = {
    Color(0, 0, 0), Color(8, 8, 8), Color(32, 32, 32), Color(64, 64, 64), Color(64, 20, 18), Color(64, 3, 0), Color(23, 1, 0), Color(7, 0, 0),
    Color(64, 48, 25), Color(64, 22, 0), Color(23, 8, 0), Color(10, 7, 0), Color(64, 64, 9), Color(64, 64, 0), Color(23, 23, 0), Color(6, 6, 0),
    Color(33, 64, 11), Color(17, 64, 0), Color(6, 23, 0), Color(5, 11, 0), Color(14, 64, 11), Color(0, 64, 0), Color(0, 23, 0), Color(0, 7, 0),
    Color(13, 64, 18), Color(0, 64, 0), Color(0, 23, 0), Color(0, 7, 0), Color(13, 64, 32), Color(0, 64, 15), Color(0, 23, 6), Color(0, 8, 4),
    Color(12, 64, 45), Color(0, 64, 37), Color(0, 23, 13), Color(0, 7, 4), Color(15, 48, 64), Color(0, 42, 64), Color(0, 17, 21), Color(0, 5, 7),
    Color(17, 34, 64), Color(0, 21, 64), Color(0, 7, 23), Color(0, 2, 7), Color(18, 18, 64), Color(0, 0, 64), Color(0, 0, 23), Color(0, 0, 7),
    Color(33, 18, 64), Color(21, 0, 64), Color(6, 0, 26), Color(3, 0, 13), Color(64, 19, 64), Color(64, 0, 64), Color(23, 0, 23), Color(7, 0, 7),
    Color(64, 20, 34), Color(64, 2, 21), Color(23, 1, 7), Color(9, 0, 5), Color(64, 7, 0), Color(39, 14, 0), Color(31, 21, 0), Color(16, 26, 0),
    Color(0, 15, 0), Color(0, 22, 13), Color(0, 21, 32), Color(0, 0, 64), Color(0, 18, 20), Color(7, 0, 53), Color(32, 32, 32), Color(9, 9, 9),
    Color(64, 3, 0), Color(47, 64, 0), Color(43, 60, 0), Color(22, 64, 0), Color(0, 35, 0), Color(0, 64, 31), Color(0, 42, 64), Color(0, 7, 64),
    Color(14, 0, 64), Color(30, 0, 64), Color(46, 6, 32), Color(17, 9, 0), Color(64, 19, 0), Color(33, 57, 0), Color(26, 64, 0), Color(0, 64, 0),
    Color(0, 64, 0), Color(18, 64, 25), Color(0, 64, 51), Color(21, 34, 64), Color(10, 20, 51), Color(33, 31, 60), Color(53, 4, 64), Color(64, 2, 23),
    Color(64, 32, 0), Color(47, 45, 0), Color(35, 64, 0), Color(33, 24, 0), Color(14, 11, 0), Color(4, 20, 2), Color(0, 21, 14), Color(5, 5, 11),
    Color(5, 8, 23), Color(27, 16, 6), Color(44, 2, 0), Color(64, 21, 14), Color(56, 27, 0), Color(64, 64, 0), Color(39, 57, 0), Color(24, 46, 0),
    Color(7, 7, 13), Color(56, 64, 21), Color(30, 64, 47), Color(38, 38, 64), Color(35, 25, 64), Color(17, 17, 17), Color(30, 30, 30), Color(56, 64, 64),
    Color(42, 2, 0), Color(14, 0, 0), Color(0, 53, 0), Color(0, 17, 0), Color(47, 45, 0), Color(16, 13, 0), Color(46, 24, 0), Color(19, 6, 0) };

Launchpad::Launchpad( ApplicationController& applicationController, grid::Grid& grid, AdditionalButtons& additionalButtons,
    RotaryControls& rotaryControls, lcd::Lcd& lcd, midi::UsbMidi& usbMidi ) :
        Application( applicationController ),
        gui_( LcdGui( *this, lcd ) ),
        grid_( grid ),
        usbMidi_( usbMidi ),
        applicationEnded_( true ),
        mode_( Launchpad95Mode_UNKNOWN ),
        submode_( Launchpad95Submode_DEFAULT ),
        layout_( Layout_SESSION ),
        isPlaying_( false ),
        isRecording_( false ),
        isSessionRecording_( false ),
        nudgeDownActive_( false ),
        nudgeUpActive_( false ),
        tempo_( 0 ),
        signatureNumerator_( 0 ),
        signatureDenominator_( 0 ),
        incomingSystemExclusiveMessageLength_( 0 )
{
    const int16_t initialControlValue = midi::kMaximumControlValue / 2;
    rotaryControlValue_[0] = initialControlValue;
    rotaryControlValue_[1] = initialControlValue;
}

void Launchpad::run( ApplicationThread& thread )
{
    grid_.discardAllPendingButtonEvents();
    grid_.turnAllLedsOff();
    gui_.initialize();

    sendMixerModeControlMessage();

    enableAdditionalButtonInputHandler();
    enableGridInputHandler();
    enableMidiInputHandler();
    enableRotaryControlInputHandler();

    applicationEnded_ = false;
    while (!applicationEnded_)
    {
        gui_.refresh();
        thread.DelayUntil( LcdGui::refreshPeriodMs );
    }
}

Launchpad95Mode Launchpad::determineLaunchpad95Mode()
{
    Launchpad95Mode mode = Launchpad95Mode_UNKNOWN;

    do
    {
        Color color = grid_.getLedColor( kDeviceControlColumn, kSession.positionY );
        if (kLaunchpadColorPalette[21] == color)
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kUser1.positionY );
        if (kLaunchpadColorPalette[37] == color)
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        else if (kLaunchpadColorPalette[48] == color)
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        else if (kLaunchpadColorPalette[45] == color)
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kUser2.positionY );
        if (kLaunchpadColorPalette[53] == color)
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        else if (kLaunchpadColorPalette[9] == color)
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        else if (kLaunchpadColorPalette[45] == color)
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        color = grid_.getLedColor( kDeviceControlColumn, kMixer.positionY );
        if (kLaunchpadColorPalette[29] == color)
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

    switch (mode_)
    {
        case Launchpad95Mode_INSTRUMENT:
            color = grid_.getLedColor( kSubmodeColumn, 7 );
            if (kLaunchpadColorPalette[9] == color)
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            color = grid_.getLedColor( kSubmodeColumn, 7 );
            if (kLaunchpadColorPalette[5] == color)
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_MIXER:
            do
            {
                color = grid_.getLedColor( kSubmodeColumn, 7 );
                if (kLaunchpadColorPalette[31] == color)
                {
                    submode = Launchpad95Submode_VOLUME;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 6 );
                if (kLaunchpadColorPalette[31] == color)
                {
                    submode = Launchpad95Submode_PAN;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 5 );
                if (kLaunchpadColorPalette[31] == color)
                {
                    submode = Launchpad95Submode_SEND_A;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 4 );
                if (kLaunchpadColorPalette[31] == color)
                {
                    submode = Launchpad95Submode_SEND_B;
                }
            } while (false);
            break;
        case Launchpad95Mode_MELODIC_SEQUENCER:
            do
            {
                color = grid_.getLedColor( kSubmodeColumn, 3 );
                if (kLaunchpadColorPalette[29] == color)
                {
                    submode = Launchpad95Submode_LENGTH;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 2 );
                if (kLaunchpadColorPalette[48] == color)
                {
                    submode = Launchpad95Submode_OCTAVE;
                    break;
                }
                color = grid_.getLedColor( kSubmodeColumn, 1 );
                if (kLaunchpadColorPalette[37] == color)
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

void Launchpad::handleRotaryControlEvent( const RotaryControls::Event event )
{
    rotaryControlValue_[event.control] += event.steps;

    if (rotaryControlValue_[event.control] > midi::kMaximumControlValue)
    {
        rotaryControlValue_[event.control] = midi::kMaximumControlValue;
    }
    else if (rotaryControlValue_[event.control] < midi::kMinimumControlValue)
    {
        rotaryControlValue_[event.control] = midi::kMinimumControlValue;
    }
    usbMidi_.sendControlChange( kAdditionalControlMidiChannel, event.control, rotaryControlValue_[event.control] );
    gui_.registerMidiOutputActivity();
    gui_.displayRotaryControlValues();
}

void Launchpad::handleAdditionalButtonEvent( const AdditionalButtons::Event event )
{
    if (AdditionalButtons::extraNoteButton == event.button) // only send note on the event of black button
    {
        const uint8_t controlValue = (ButtonAction_PRESSED == event.action) ? kControlValueHigh : kControlValueLow;
        usbMidi_.sendNoteOn( kAdditionalControlMidiChannel, kAdditionalNoteButtonNote, controlValue );
        gui_.registerMidiOutputActivity();
    }
    else if (AdditionalButtons::internalMenuButton == event.button)
    {
        if (ButtonAction_PRESSED == event.action)
        {
            applicationEnded_ = true;
            switchApplication( ApplicationIndex_INTERNAL_MENU );
        }
    }
}

void Launchpad::handleMidiPacket( const midi::MidiPacket packet )
{
    const uint8_t codeIndexNumber = packet.header & midi::kCodeIndexNumberMask;
    switch (codeIndexNumber)
    {
        case midi::kNoteOn:
            processNoteOnMidiMessage( packet.data[0] & midi::kChannelMask, packet.data[1], packet.data[2] );
            break;
        case midi::kNoteOff:
            processNoteOnMidiMessage( packet.data[0] & midi::kChannelMask, packet.data[1], 0 );
            break;
        case midi::kControlChange:
            processChangeControlMidiMessage( packet.data[0] & midi::kChannelMask, packet.data[1], packet.data[2] );
            break;
        case midi::kSystemExclusive:
        case midi::kSystemExclusiveEnd1Byte:
        case midi::kSystemExclusiveEnd2Bytes:
        case midi::kSystemExclusiveEnd3Bytes:
            processSystemExclusiveMidiPacket( packet );
            break;
        default:
            break;
    }
    gui_.registerMidiInputActivity();
}

void Launchpad::handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    const uint8_t controlValue = (ButtonAction_PRESSED == event.action) ? kControlValueHigh : kControlValueLow;
    if (kDeviceControlColumn == event.positionX)
    {
        usbMidi_.sendControlChange( kDeviceControlMidiChannel, kDeviceControlColumnValue[event.positionY], controlValue );
    }
    else
    {
        switch (layout_)
        {
            case Layout_USER1:
                usbMidi_.sendNoteOn( kUser1LayoutMidiChannel, kDrumLayout[event.positionX][event.positionY], controlValue );
                break;
            case Layout_USER2:
                usbMidi_.sendNoteOn( kUser2LayoutMidiChannel, kSessionLayout[event.positionX][event.positionY], controlValue );
                break;
            case Layout_SESSION:
            default:
                usbMidi_.sendNoteOn( kSessionLayoutMidiChannel, kSessionLayout[event.positionX][event.positionY], controlValue );
                break;
        }
    }
    gui_.registerMidiOutputActivity();
}

void Launchpad::processDawInfoMessage( const char* const message )
{
    switch (message[0])
    {
        case 't':
            strncpy( trackName_, &message[1], kMaximumDawInfoStringLength );
            break;
        case 'c':
            strncpy( clipName_, &message[1], kMaximumDawInfoStringLength );
            break;
        case 'd':
            strncpy( deviceName_, &message[1], kMaximumDawInfoStringLength );
            break;
        case 's':
            {
                isPlaying_ = ('P' == message[1]);
                isRecording_ = ('R' == message[2]);
                isSessionRecording_ = ('S' == message[3]);
            }
            break;
        case 'T':
            {
                tempo_ = (message[1] - '0')*100 + (message[2] - '0')*10 + (message[3] - '0');
                signatureNumerator_ = (message[4] - '0')*10 + (message[5] - '0');
                signatureDenominator_ = (message[6] - '0')*10 + (message[7] - '0');
                nudgeDownActive_ = ('D' == message[8]);
                nudgeUpActive_ = ('U' == message[8]);
            }
            break;
        default:
            break;

        gui_.refresh();
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
            mode_ = determineLaunchpad95Mode();
            // only melodic step sequencer can stay in submode between mode changes
            submode_ = (Launchpad95Mode_MELODIC_SEQUENCER == mode_) ? determineLaunchpad95Submode() : Launchpad95Submode_DEFAULT;

            gui_.refresh();
        }
    }
}

void Launchpad::processNoteOnMidiMessage( uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    uint8_t ledPositionX, ledPositionY;
    bool ledPositionCorrect = false;

    if (Layout_USER1 == layout_)
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
        if (velocity < kLaunchpadColorPalette.size())
        {
            grid::LedLightingType lightingType = grid::LedLightingType_LIGHT;
            if (channel <= static_cast<uint8_t>(grid::LedLightingType_PULSE))
            {
                lightingType = static_cast<grid::LedLightingType>(channel);
            }

            grid_.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[velocity], lightingType );

            if ((Layout_USER1 != layout_) && (kSubmodeColumn == ledPositionX))
            {
                // possible change in submode
                submode_ = determineLaunchpad95Submode();
                gui_.refresh();
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
                            layout_ = static_cast<Layout>(layoutIndex);
                        }
                    }
                    break;
                case kChallenge:
                    usbMidi_.sendSystemExclussive( &kChallengeResponse[0], kChallengeResponseLength );
                    gui_.registerMidiOutputActivity();
                    break;
                case kTextScroll:
                    message[length-1] = 0; // put string terminator at the end
                    processDawInfoMessage( reinterpret_cast<char*>(&message[7]) );
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

    if (incomingSystemExclusiveMessageLength_ >= (kSystemExclussiveMessageMaximumLength_ - 3))
    {
        incomingSystemExclusiveMessageLength_ = 0; // discard this message, as it is too long
    }
    else
    {
        switch (codeIndexNumber)
        {
            case midi::kSystemExclusive:
            case midi::kSystemExclusiveEnd3Bytes:
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[1];
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[2];
                break;
            case midi::kSystemExclusiveEnd1Byte:
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[0];
                break;
            case midi::kSystemExclusiveEnd2Bytes:
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[incomingSystemExclusiveMessageLength_++] = packet.data[1];
                break;
            default:
                break;
        }

        if ((midi::kSystemExclusiveEnd1Byte == codeIndexNumber) || (midi::kSystemExclusiveEnd2Bytes == codeIndexNumber) ||
                (midi::kSystemExclusiveEnd3Bytes == codeIndexNumber))
        {
            processSystemExclusiveMessage( &systemExclusiveInputMessage_[0], incomingSystemExclusiveMessageLength_ );
            incomingSystemExclusiveMessageLength_ = 0; // reset message length
        }
    }
}

void Launchpad::sendMixerModeControlMessage()
{
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueHigh );
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueLow );
    gui_.registerMidiOutputActivity();
}

}
} // namespace
