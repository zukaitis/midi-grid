#ifndef PROGRAM_LAUNCHPAD_H_
#define PROGRAM_LAUNCHPAD_H_

#include "Types.h"
#include "usb/UsbMidi.h"

namespace grid
{
    class Grid;
    class Switches;
}

namespace lcd
{
    class Gui;
}

namespace midi
{
    class UsbMidi;
}

namespace launchpad
{

enum Layout
{
    Layout_SESSION = 0,
    Layout_USER1,
    Layout_USER2,
    Layout_RESERVED,
    Layout_VOLUME,
    Layout_PAN,
    kMaximumLayoutIndex = Layout_PAN
};

enum Launchpad95Mode
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

enum Launchpad95Submode
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

class Launchpad
{
public:
    Launchpad( grid::Grid& grid_, grid::Switches& switches_, lcd::Gui& gui_, midi::UsbMidi& usbMidi_ );

    void runProgram();

private:
    Launchpad95Mode determineLaunchpad95Mode();
    Launchpad95Submode determineLaunchpad95Submode();

    bool handleAdditionalControlInput();
    bool handleGridInput();
    bool handleMidiInput();

    void processDawInfoMessage( const char* const message, uint8_t length );
    void processChangeControlMidiMessage( const uint8_t channel, const uint8_t control, const uint8_t value );
    void processNoteOnMidiMessage( uint8_t channel, uint8_t note, uint8_t velocity );
    void processSystemExclusiveMessage( uint8_t* const message, uint8_t length );
    void processSystemExclusiveMidiPacket( const midi::MidiPacket& packet );

    void sendMixerModeControlMessage();
    void setCurrentLayout( const uint8_t layout );

    grid::Grid& grid_;
    grid::Switches& switches_;
    lcd::Gui& gui_;
    midi::UsbMidi& usbMidi_;

    Launchpad95Mode currentLaunchpad95Mode_; // used only to identify submode
    Layout currentLayout_;

    static const uint8_t systemExclussiveMessageMaximumLength_ = 64;

    uint8_t systemExclusiveInputMessage_[systemExclussiveMessageMaximumLength_];
    uint8_t systemExclusiveInputMessageLength_;

    int16_t rotaryControlValue_[2];
};

} // namespace

#endif // PROGRAM_LAUNCHPAD_H_
