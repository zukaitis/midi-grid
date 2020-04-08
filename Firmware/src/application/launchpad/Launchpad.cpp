#include "application/launchpad/Launchpad.hpp"

#include "grid/GridInterface.h"
#include "additional_buttons/AdditionalButtonsInterface.h"
#include "rotary_controls/RotaryControlsInterface.h"
#include "lcd/LcdInterface.h"
#include "system/System.hpp"
#include "testing/TestingInterface.h"

#include <cstring>
#include <etl/array.h>
#include <etl/string_view.h>

#include "syslog/info.h"

namespace application
{
namespace launchpad
{

/* MIDI */
static const uint8_t kControlValueLow = 0;
static const uint8_t kControlValueHigh = 127;

static const etl::array<uint8_t, 10> kChallengeResponse = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7 };

static const etl::string<6> kStandardSystemExclussiveMessageHeader = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x02, 0x18 };
static const etl::string_view standardSystemExclussiveMessageHeaderView( kStandardSystemExclussiveMessageHeader );
static const uint8_t kStandardSystemExclussiveMessageMinimumLength = 8;

static const etl::string<9> systemExclussiveBootloaderMessage = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x00, 0x71, 0x00, 0x69,
    static_cast<char>(0xF7) };
static const etl::string_view systemExclussiveBootloaderMessageView( systemExclussiveBootloaderMessage );

enum class LaunchpadSysExCommand
{
    SET_LED = 0x0A,
    SET_LED_RGB = 0x0B,
    SET_LEDS_BY_COLUMN = 0x0C,
    SET_LEDS_BY_ROW = 0x0D,
    SET_ALL_LEDS = 0x0E,
    TEXT_SCROLL = 0x14,
    SET_LAYOUT = 0x22,
    FLASH_LED = 0x23,
    PULSE_LED = 0x28,
    FADER_SETUP = 0x2B,
    DISCONNECT_OR_CHALLENGE = 0x40
};

static const etl::string<6> customSysExMessageHeader = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x02, 0x07 };
static const etl::string_view customSysExMessageHeaderView( customSysExMessageHeader );

enum class CustomSysExCommand
{
    INJECT_BUTTON_PRESS = 0
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
static const etl::array<uint8_t, kNumberOfRows> kDeviceControlColumnValue = {
        kUser2.controlValue, kMixer.controlValue, kUser1.controlValue, kSession.controlValue,
        kUp.controlValue, kLeft.controlValue, kRight.controlValue, kDown.controlValue };

static const etl::array<etl::array<uint8_t, kNumberOfRows>, kNumberOfColumns> kSessionLayout = {{
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} }};

static const etl::array<etl::array<uint8_t, kNumberOfRows>, kNumberOfColumns> kDrumLayout = {{
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} }};

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

Launchpad::Launchpad( ApplicationController* applicationController, grid::GridInterface* grid,
    additional_buttons::AdditionalButtonsInterface* additionalButtons, rotary_controls::RotaryControlsInterface* rotaryControls,
    lcd::LcdInterface* lcd, midi::UsbMidi* usbMidi, mcu::System* system, testing::TestingInterface* testing ) :
        Application( *applicationController ),
        gui_( *this, *lcd ),
        grid_( *grid ),
        usbMidi_( *usbMidi ),
        system_( *system ),
        testing_( *testing ),
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
        signatureDenominator_( 0 )
{
    const int16_t initialControlValue = midi::kMaximumControlValue / 2;
    rotaryControlValue_.at( 0 ) = initialControlValue;
    rotaryControlValue_.at( 1 ) = initialControlValue;
}

void Launchpad::run( ApplicationThread& thread )
{
    grid_.discardPendingInput();
    grid_.turnAllLedsOff();
    gui_.initialize();

    enableRotaryControlInputHandler();
    enableAdditionalButtonInputHandler();
    enableGridInputHandler();
    enableMidiInputHandler();

    sendMixerModeControlMessage();

    applicationEnded_ = false;
    while (!applicationEnded_)
    {
        gui_.refresh();
        thread.delay( LcdGui::refreshPeriodMs );
    }
}

Launchpad95Mode Launchpad::determineLaunchpad95Mode()
{
    Launchpad95Mode mode = Launchpad95Mode_UNKNOWN;

    do
    {
        Color color = grid_.getLedColor( {kDeviceControlColumn, kSession.positionY} );
        if (kLaunchpadColorPalette.at( 21 ) == color)
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        color = grid_.getLedColor( {kDeviceControlColumn, kUser1.positionY} );
        if (kLaunchpadColorPalette.at( 37 ) == color)
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        if (kLaunchpadColorPalette.at( 48 ) == color)
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        if (kLaunchpadColorPalette.at( 45 ) == color)
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        color = grid_.getLedColor( {kDeviceControlColumn, kUser2.positionY} );
        if (kLaunchpadColorPalette.at( 53 ) == color)
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        if (kLaunchpadColorPalette.at( 9 ) == color)
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        if (kLaunchpadColorPalette.at( 45 ) == color)
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        color = grid_.getLedColor( {kDeviceControlColumn, kMixer.positionY} );
        if (kLaunchpadColorPalette.at( 29 ) == color)
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
            color = grid_.getLedColor( {kSubmodeColumn, 7} );
            if (kLaunchpadColorPalette.at( 9 ) == color)
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            color = grid_.getLedColor( {kSubmodeColumn, 7} );
            if (kLaunchpadColorPalette.at( 5 ) == color)
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_MIXER:
            do
            {
                color = grid_.getLedColor( {kSubmodeColumn, 7} );
                if (kLaunchpadColorPalette.at( 31 ) == color)
                {
                    submode = Launchpad95Submode_VOLUME;
                    break;
                }
                color = grid_.getLedColor( {kSubmodeColumn, 6});
                if (kLaunchpadColorPalette.at( 31 ) == color)
                {
                    submode = Launchpad95Submode_PAN;
                    break;
                }
                color = grid_.getLedColor( {kSubmodeColumn, 5} );
                if (kLaunchpadColorPalette.at( 31 ) == color)
                {
                    submode = Launchpad95Submode_SEND_A;
                    break;
                }
                color = grid_.getLedColor( {kSubmodeColumn, 4} );
                if (kLaunchpadColorPalette.at( 31 ) == color)
                {
                    submode = Launchpad95Submode_SEND_B;
                }
            } while (false);
            break;
        case Launchpad95Mode_MELODIC_SEQUENCER:
            do
            {
                color = grid_.getLedColor( {kSubmodeColumn, 3} );
                if (kLaunchpadColorPalette.at( 29 ) == color)
                {
                    submode = Launchpad95Submode_LENGTH;
                    break;
                }
                color = grid_.getLedColor( {kSubmodeColumn, 2} );
                if (kLaunchpadColorPalette.at( 48 ) == color)
                {
                    submode = Launchpad95Submode_OCTAVE;
                    break;
                }
                color = grid_.getLedColor( {kSubmodeColumn, 1} );
                if (kLaunchpadColorPalette.at( 37 ) == color)
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

void Launchpad::handleRotaryControlEvent( const rotary_controls::Event& event )
{
    rotaryControlValue_.at( event.control ) += event.steps;

    if (rotaryControlValue_.at( event.control ) > midi::kMaximumControlValue)
    {
        rotaryControlValue_.at( event.control ) = midi::kMaximumControlValue;
    }
    else if (rotaryControlValue_.at( event.control ) < midi::kMinimumControlValue)
    {
        rotaryControlValue_.at( event.control ) = midi::kMinimumControlValue;
    }
    usbMidi_.sendControlChange( kAdditionalControlMidiChannel, event.control, rotaryControlValue_.at( event.control ) );
    gui_.registerMidiOutputActivity();
    gui_.displayRotaryControlValues();
}

void Launchpad::handleAdditionalButtonEvent( const additional_buttons::Event& event )
{
    if (additional_buttons::Button::extraNote == event.button) // only send note on the event of black button
    {
        const uint8_t controlValue = (ButtonAction::PRESSED == event.action) ? kControlValueHigh : kControlValueLow;
        usbMidi_.sendNoteOn( kAdditionalControlMidiChannel, kAdditionalNoteButtonNote, controlValue );
        gui_.registerMidiOutputActivity();
    }
    else if (additional_buttons::Button::internalMenu == event.button)
    {
        if (ButtonAction::PRESSED == event.action)
        {
            applicationEnded_ = true;
            switchApplication( ApplicationIndex_INTERNAL_MENU );
        }
    }
}

void Launchpad::handleMidiPacket( const midi::MidiPacket& packet )
{
    const uint8_t codeIndexNumber = packet.header & midi::kCodeIndexNumberMask;
    switch (codeIndexNumber)
    {
        case midi::kNoteOn:
            processNoteOnMidiMessage( packet.data.at( 0 ) & midi::kChannelMask, packet.data.at( 1 ), packet.data.at( 2 ) );
            break;
        case midi::kNoteOff:
            processNoteOnMidiMessage( packet.data.at( 0 ) & midi::kChannelMask, packet.data.at( 1 ), 0 );
            break;
        case midi::kControlChange:
            processChangeControlMidiMessage( packet.data.at( 0 ) & midi::kChannelMask, packet.data.at( 1 ), packet.data.at( 2 ) );
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

void Launchpad::handleGridButtonEvent( const grid::ButtonEvent& event )
{
    const uint8_t controlValue = (ButtonAction::PRESSED == event.action) ? kControlValueHigh : kControlValueLow;
    if (kDeviceControlColumn == event.coordinates.x)
    {
        usbMidi_.sendControlChange( kDeviceControlMidiChannel, kDeviceControlColumnValue.at( event.coordinates.y ), controlValue );
    }
    else
    {
        switch (layout_)
        {
            case Layout_USER1:
                usbMidi_.sendNoteOn( kUser1LayoutMidiChannel, kDrumLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
                break;
            case Layout_USER2:
                usbMidi_.sendNoteOn( kUser2LayoutMidiChannel, kSessionLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
                break;
            case Layout_SESSION:
            default:
                usbMidi_.sendNoteOn( kSessionLayoutMidiChannel, kSessionLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
                break;
        }
    }
    gui_.registerMidiOutputActivity();
}

void Launchpad::processDawInfoMessage( const etl::string_view& message )
{
    switch (message.at( 0 ))
    {
        case 't':
            trackName_.assign( &message.at( 1 ) );
            break;
        case 'c':
            clipName_.assign( &message.at( 1 ) );
            break;
        case 'd':
            deviceName_.assign( &message.at( 1 ) );
            break;
        case 's':
            {
                isPlaying_ = ('P' == message.at( 1 ));
                isRecording_ = ('R' == message.at( 2 ));
                isSessionRecording_ = ('S' == message.at( 3 ));
            }
            break;
        case 'T':
            {
                // tempo_ = (message.at( 1 ) - '0')*100 + (message.at( 2 ) - '0')*10 + (message.at( 3 ) - '0');
                // signatureNumerator_ = (message.at( 4 ) - '0')*10 + (message.at( 5 ) - '0');
                // signatureDenominator_ = (message.at( 6 ) - '0')*10 + (message.at( 7 ) - '0');
                nudgeDownActive_ = ('D' == message.at( 8 ));
                nudgeUpActive_ = ('U' == message.at( 8 ));
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
            if (kDeviceControlColumnValue.at( y ) == control)
            {
                const uint8_t ledPositionY = y;
                grid_.setLed( {ledPositionX, ledPositionY}, kLaunchpadColorPalette.at( value ), static_cast<grid::LedLightingType>(channel) );
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
    uint8_t ledPositionX;
    uint8_t ledPositionY;
    bool ledPositionCorrect = false;

    if (Layout_USER1 == layout_)
    {
        // only this layout uses drum layout
        if (note >= kDrumLayout.at( 0 ).at( 0 ))
        {
            if (note <= kDrumLayout.at( 3 ).at( 7 ))
            {
                ledPositionX = note % 4;
                ledPositionY = (note - kDrumLayout.at( 0 ).at( 0 )) / 4;
                ledPositionCorrect = true;
            }
            else if (note <= kDrumLayout.at( 7 ).at( 7 ))
            {
                ledPositionX = (note % 4) + 4;
                ledPositionY = (note - kDrumLayout.at( 4 ).at( 0 )) / 4;
                ledPositionCorrect = true;
            }
            else if (note <= kDrumLayout.at( 8 ).at( 0 ))
            {
                ledPositionX = 8;
                ledPositionY = kDrumLayout.at( 8 ).at( 0 ) - note;
                ledPositionCorrect = true;
            }
        }
    }
    else
    {
        if ((note >= kSessionLayout.at( 0 ).at( 0 )) && (note <= kSessionLayout.at( 8 ).at( 7 )))
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
            grid::LedLightingType lightingType = grid::LedLightingType::LIGHT;
            if (channel <= static_cast<uint8_t>(grid::LedLightingType::PULSE))
            {
                lightingType = static_cast<grid::LedLightingType>(channel);
            }

            grid_.setLed( {ledPositionX, ledPositionY}, kLaunchpadColorPalette.at( velocity ), lightingType );

            if ((Layout_USER1 != layout_) && (kSubmodeColumn == ledPositionX))
            {
                // possible change in submode
                submode_ = determineLaunchpad95Submode();
                gui_.refresh();
            }
        }
    }
}

void Launchpad::processSystemExclusiveMessage( const SystemExclussiveMessage& message )
{
    if (message.size() >= kStandardSystemExclussiveMessageMinimumLength)
    {
        const etl::string_view header( message.begin(), kStandardSystemExclussiveMessageHeader.size() );
        if (header == standardSystemExclussiveMessageHeaderView)
        {
            const auto command = static_cast<LaunchpadSysExCommand>(message.at( 6 ));
            switch (command)
            {
                case LaunchpadSysExCommand::SET_ALL_LEDS:
                    if (0 == message.at( 7 ))
                    {
                        grid_.turnAllLedsOff();
                    }
                    break;
                case LaunchpadSysExCommand::SET_LAYOUT:
                    {
                        const uint8_t layoutIndex = message.at( 7 );
                        if (layoutIndex <= kMaximumLayoutIndex)
                        {
                            layout_ = static_cast<Layout>(layoutIndex);
                        }
                    }
                    break;
                case LaunchpadSysExCommand::DISCONNECT_OR_CHALLENGE:
                    if (12 == message.size()) // challenge
                    {
                        usbMidi_.sendSystemExclussive( &kChallengeResponse.at( 0 ), kChallengeResponse.size() );
                        gui_.registerMidiOutputActivity();
                    }
                    else if (8 == message.size()) // disconnect
                    {
                        applicationEnded_ = true;
                        Application::switchApplication( ApplicationIndex_GRID_TEST );
                    }
                    break;
                case LaunchpadSysExCommand::TEXT_SCROLL:
                    {
                        const etl::string_view dawInfo( &message.at( 7 ), message.end() );
                        processDawInfoMessage( dawInfo );
                    }
                    break;
                default:
                    break;
            }
        }
        else if (header == customSysExMessageHeaderView)
        {
            const auto command = static_cast<CustomSysExCommand>(message.at( 6 ));
            if (CustomSysExCommand::INJECT_BUTTON_PRESS == command)
            {
                if (10 == message.size())
                {
                    const ::Coordinates button = { static_cast<uint8_t>(message.at( 7 )),
                        static_cast<uint8_t>(message.at( 8 )) };
                    testing_.injectButtonPress( button );
                }
            }
        }
        else
        {
            const etl::string_view messageView( message.begin(), systemExclussiveBootloaderMessage.size() );

            if (messageView == systemExclussiveBootloaderMessageView)
            {
                // reset into DFU bootloader
                system_.resetIntoBootloader();
            }
        }
    }
}

void Launchpad::processSystemExclusiveMidiPacket( const midi::MidiPacket& packet )
{
    const uint8_t codeIndexNumber = packet.header & midi::kCodeIndexNumberMask;

    if (systemExclusiveInputMessage_.size() >= (systemExclusiveInputMessage_.max_size() - 3))
    {
        systemExclusiveInputMessage_.clear(); // discard this message, as it is too long
    }
    else
    {
        switch (codeIndexNumber)
        {
            case midi::kSystemExclusive:
            case midi::kSystemExclusiveEnd3Bytes:
                systemExclusiveInputMessage_.push_back( packet.data.at( 0 ) );
                systemExclusiveInputMessage_.push_back( packet.data.at( 1 ) );
                systemExclusiveInputMessage_.push_back( packet.data.at( 2 ) );
                break;
            case midi::kSystemExclusiveEnd1Byte:
                systemExclusiveInputMessage_.push_back( packet.data.at( 0 ) );
                break;
            case midi::kSystemExclusiveEnd2Bytes:
                systemExclusiveInputMessage_.push_back( packet.data.at( 0 ) );
                systemExclusiveInputMessage_.push_back( packet.data.at( 1 ) );
                break;
            default:
                break;
        }

        if ((midi::kSystemExclusiveEnd1Byte == codeIndexNumber) ||
            (midi::kSystemExclusiveEnd2Bytes == codeIndexNumber) ||
            (midi::kSystemExclusiveEnd3Bytes == codeIndexNumber))
        {
            processSystemExclusiveMessage( systemExclusiveInputMessage_ );
            systemExclusiveInputMessage_.clear(); // reset message length
        }
    }
}

void Launchpad::sendMixerModeControlMessage()
{
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueHigh );
    usbMidi_.sendControlChange( kDeviceControlMidiChannel, kMixer.controlValue, kControlValueLow );
    gui_.registerMidiOutputActivity();
}

}  // namespace launchpad
}  // namespace application
