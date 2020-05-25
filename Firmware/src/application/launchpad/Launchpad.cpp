#include "application/launchpad/Launchpad.hpp"
#include "application/launchpad/Assets.h"

#include "grid/GridInterface.h"
#include "additional_buttons/AdditionalButtonsInterface.h"
#include "rotary_controls/RotaryControlsInterface.h"
#include "lcd/LcdInterface.h"
#include "system/System.hpp"
#include "testing/TestingInterface.h"

#include <cstring>
#include <etl/array.h>
#include <etl/string_view.h>

namespace application
{
namespace launchpad
{

Launchpad::Launchpad( ApplicationController* applicationController, grid::GridInterface* grid,
    additional_buttons::AdditionalButtonsInterface* additionalButtons, rotary_controls::RotaryControlsInterface* rotaryControls,
    lcd::LcdInterface* lcd, midi::UsbMidi* usbMidi, mcu::System* system, testing::TestingInterface* testing ) :
        Application( *applicationController ),
        gui_( this, lcd ),
        grid_( *grid ),
        usbMidi_( *usbMidi ),
        system_( *system ),
        testing_( *testing ),
        applicationEnded_( true ),
        mode_( Mode::UNKNOWN ),
        submode_( Submode::DEFAULT ),
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

Mode Launchpad::determineMode()
{
    Mode mode = Mode::UNKNOWN;

    for (const auto& a : modeAttributes)
    {
        if (a.second.color == grid_.getLedColor( a.second.definingLed ))
        {
            mode = a.first;
            break;
        }
    }

    return mode;
}

Submode Launchpad::determineSubmode()
{
    Submode submode = Submode::DEFAULT;

    for (const auto& a : submodeAttributes)
    {
        if (a.second.mode == mode_)
        {
            if (a.second.color == grid_.getLedColor( a.second.definingLed ))
            {
                submode = a.first;
                break;
            }
        }
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
    usbMidi_.sendControlChange( static_cast<uint8_t>(MidiChannel::ADDITIONAL_CONTROL),
        event.control, rotaryControlValue_.at( event.control ) );
    gui_.registerMidiOutputActivity();
}

void Launchpad::handleAdditionalButtonEvent( const additional_buttons::Event& event )
{
    if (additional_buttons::Button::extraNote == event.button) // only send note on the event of black button
    {
        const uint8_t controlValue = (ButtonAction::PRESSED == event.action) ? kControlValueHigh : kControlValueLow;
        usbMidi_.sendNoteOn( static_cast<uint8_t>(MidiChannel::ADDITIONAL_CONTROL), kAdditionalNoteButtonNote, controlValue );
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
        usbMidi_.sendControlChange( static_cast<uint8_t>(MidiChannel::DEVICE_CONTROL),
            kDeviceControlColumnValue.at( event.coordinates.y ), controlValue );
    }
    else
    {
        switch (layout_)
        {
            case Layout_USER1:
                usbMidi_.sendNoteOn( static_cast<uint8_t>(MidiChannel::USER1_LAYOUT),
                    kDrumLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
                break;
            case Layout_USER2:
                usbMidi_.sendNoteOn( static_cast<uint8_t>(MidiChannel::USER2_LAYOUT),
                    kSessionLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
                break;
            case Layout_SESSION:
            default:
                usbMidi_.sendNoteOn( static_cast<uint8_t>(MidiChannel::SESSION_LAYOUT),
                    kSessionLayout.at( event.coordinates.x ).at( event.coordinates.y ), controlValue );
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
                tempo_ = (message.at( 1 ) - '0')*100 + (message.at( 2 ) - '0')*10 + (message.at( 3 ) - '0');
                signatureNumerator_ = (message.at( 4 ) - '0')*10 + (message.at( 5 ) - '0');
                signatureDenominator_ = (message.at( 6 ) - '0')*10 + (message.at( 7 ) - '0');
                nudgeDownActive_ = ('D' == message.at( 8 ));
                nudgeUpActive_ = ('U' == message.at( 8 ));
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
            if (kDeviceControlColumnValue.at( y ) == control)
            {
                const uint8_t ledPositionY = y;
                grid_.setLed( {ledPositionX, ledPositionY}, palette.at( value ), static_cast<grid::LedLightingType>(channel) );
                break;
            }
        }

        if ( (control == kSession.controlValue) ||
            (control == kUser1.controlValue) ||
            (control == kMixer.controlValue) ||
            (control == kUser2.controlValue) )
        {
            mode_ = determineMode();
            // only melodic step sequencer can stay in submode between mode changes
            submode_ = (Mode::MELODIC_SEQUENCER == mode_) ? determineSubmode() : Submode::DEFAULT;
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
        if (velocity < palette.size())
        {
            grid::LedLightingType lightingType = grid::LedLightingType::LIGHT;
            if (channel <= static_cast<uint8_t>(grid::LedLightingType::PULSE))
            {
                lightingType = static_cast<grid::LedLightingType>(channel);
            }

            grid_.setLed( {ledPositionX, ledPositionY}, palette.at( velocity ), lightingType );

            if ((Layout_USER1 != layout_) && (kSubmodeColumn == ledPositionX))
            {
                // possible change in submode
                submode_ = determineSubmode();
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
    usbMidi_.sendControlChange( static_cast<uint8_t>(MidiChannel::DEVICE_CONTROL), kMixer.controlValue, kControlValueHigh );
    usbMidi_.sendControlChange( static_cast<uint8_t>(MidiChannel::DEVICE_CONTROL), kMixer.controlValue, kControlValueLow );
    gui_.registerMidiOutputActivity();
}

}  // namespace launchpad
}  // namespace application
