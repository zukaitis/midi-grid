#pragma once

#include "application/Application.hpp"
#include "application/launchpad/LcdGui.hpp"
#include "types/Color.h"

#include <etl/array.h>
#include <etl/cstring.h>

namespace grid
{
    class GridInterface;
}

namespace additional_buttons
{
    class AdditionalButtonsInterface;
}

namespace rotary_controls
{
    class RotaryControlsInterface;
}

namespace midi
{
    class UsbMidi;
}

namespace application
{

namespace launchpad
{

enum Layout : uint8_t
{
    Layout_SESSION = 0,
    Layout_USER1,
    Layout_USER2,
    Layout_RESERVED,
    Layout_VOLUME,
    Layout_PAN,
    kMaximumLayoutIndex = Layout_PAN
};

enum Launchpad95Mode : uint8_t
{
    Launchpad95Mode_SESSION = 0,
    Launchpad95Mode_INSTRUMENT,
    Launchpad95Mode_DEVICE_CONTROLLER,
    Launchpad95Mode_USER1,
    Launchpad95Mode_DRUM_STEP_SEQUENCER,
    Launchpad95Mode_MELODIC_SEQUENCER,
    Launchpad95Mode_USER2,
    Launchpad95Mode_MIXER,
    Launchpad95Mode_UNKNOWN
};

enum Launchpad95Submode : uint8_t
{
    Launchpad95Submode_DEFAULT = 0,
    Launchpad95Submode_SCALE, // Instrument and Drum step sequencer
    Launchpad95Submode_VOLUME, // Mixer
    Launchpad95Submode_PAN, // Mixer
    Launchpad95Submode_SEND_A, // Mixer
    Launchpad95Submode_SEND_B, // Mixer
    Launchpad95Submode_LENGTH, // Melodic step sequencer
    Launchpad95Submode_OCTAVE, // Melodic step sequencer
    Launchpad95Submode_VELOCITY // Melodic step sequencer
};

class Launchpad : public Application
{
public:
    friend class LcdGui; // allow GUI to read info

    Launchpad( ApplicationController& applicationController, grid::GridInterface& grid, additional_buttons::AdditionalButtonsInterface& additionalButtons,
        rotary_controls::RotaryControlsInterface& rotaryControls, lcd::LcdInterface& lcd, midi::UsbMidi& usbMidi );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const additional_buttons::Event event );
    void handleGridButtonEvent( const grid::ButtonEvent event );
    void handleMidiPacket( const midi::MidiPacket packet );
    void handleRotaryControlEvent( const rotary_controls::Event event );

    Launchpad95Mode determineLaunchpad95Mode();
    Launchpad95Submode determineLaunchpad95Submode();

    void processDawInfoMessage( const char* const message );
    void processChangeControlMidiMessage( const uint8_t channel, const uint8_t control, const uint8_t value );
    void processNoteOnMidiMessage( uint8_t channel, uint8_t note, uint8_t velocity );
    void processSystemExclusiveMessage( uint8_t* const message, uint8_t length );
    void processSystemExclusiveMidiPacket( const midi::MidiPacket& packet );

    void sendMixerModeControlMessage();

    LcdGui gui_;
    grid::GridInterface& grid_;
    midi::UsbMidi& usbMidi_;

    bool applicationEnded_;
    Launchpad95Mode mode_;
    Launchpad95Submode submode_;
    Layout layout_;
    etl::array<int16_t, 2> rotaryControlValue_;
    bool isPlaying_;
    bool isRecording_;
    bool isSessionRecording_;
    static const uint8_t kMaximumDawInfoStringLength = 15;
    etl::string<15> clipName_;
    etl::string<15> deviceName_;
    etl::string<15> trackName_;
    bool nudgeDownActive_;
    bool nudgeUpActive_;
    uint16_t tempo_;
    uint8_t signatureNumerator_;
    uint8_t signatureDenominator_;


    static const uint8_t kSystemExclussiveMessageMaximumLength_ = 64;
    etl::array<uint8_t, 64> systemExclusiveInputMessage_;
    uint8_t incomingSystemExclusiveMessageLength_;
};

}  // namespace launchpad
}  // namespace application
