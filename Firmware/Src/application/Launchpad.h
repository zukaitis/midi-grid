#ifndef APPLICATION_LAUNCHPAD_H_
#define APPLICATION_LAUNCHPAD_H_

#include "application/Application.h"
#include "Types.h"
//#include "usb/UsbMidi.h"

namespace grid
{
    class Grid;
    class AdditionalButtons;
    class RotaryControls;
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

class AdditionalButtonInputHandler: public freertos::Thread
{
public:
    AdditionalButtonInputHandler( grid::AdditionalButtons& additionalButtons, lcd::Gui& gui, midi::UsbMidi& usbMidi, std::function<void()> stopApplicationCallback );

    void Run();

private:
    grid::AdditionalButtons& additionalButtons_;
    lcd::Gui& gui_;
    midi::UsbMidi& usbMidi_;

    std::function<void()> stopApplication_;
};

class GridInputHandler: public freertos::Thread
{
public:
    GridInputHandler( grid::Grid& grid );

    void Run();

private:
    grid::Grid& grid_;
};

class RotaryControlInputHandler: public freertos::Thread
{
public:
    RotaryControlInputHandler( grid::RotaryControls& rotaryControls, lcd::Gui& gui, midi::UsbMidi& usbMidi );

    void Run();

private:
    grid::RotaryControls& rotaryControls_;
    lcd::Gui& gui_;
    midi::UsbMidi& usbMidi_;

    int16_t rotaryControlValue_[2];
};

class MidiInputHandler: public freertos::Thread
{
public:
    MidiInputHandler( midi::UsbMidi& usbMidi, grid::Grid& grid, lcd::Gui& gui );

    void Run();

private:
    Launchpad95Mode determineLaunchpad95Mode();
    Launchpad95Submode determineLaunchpad95Submode();

    void processDawInfoMessage( const char* const message, uint8_t length );
    void processChangeControlMidiMessage( const uint8_t channel, const uint8_t control, const uint8_t value );
    void processNoteOnMidiMessage( uint8_t channel, uint8_t note, uint8_t velocity );
    void processSystemExclusiveMessage( uint8_t* const message, uint8_t length );
    void processSystemExclusiveMidiPacket( const midi::MidiPacket& packet );

    void setCurrentLayout( const Layout layout );

    midi::UsbMidi& usbMidi_;
    grid::Grid& grid_;
    lcd::Gui& gui_;

    Launchpad95Mode currentLaunchpad95Mode_; // used only to identify submode

    static const uint8_t kSystemExclussiveMessageMaximumLength_ = 64;
    uint8_t systemExclusiveInputMessage_[kSystemExclussiveMessageMaximumLength_];
    uint8_t incomingSystemExclusiveMessageLength_;
};

class Launchpad : public application::Application
{
public:
    Launchpad( grid::Grid& grid, grid::AdditionalButtons& additionalButtons, grid::RotaryControls& rotaryControls,
            lcd::Gui& gui, midi::UsbMidi& usbMidi );

    void runProgram();
    void stopApplicationCallback();

private:
    bool handleAdditionalControlInput();
    bool handleGridInput();
    bool handleMidiInput();

    void sendMixerModeControlMessage();

    grid::Grid& grid_;
    grid::AdditionalButtons& additionalButtons_;
    grid::RotaryControls& rotaryControls_;
    lcd::Gui& gui_;
    midi::UsbMidi& usbMidi_;

    AdditionalButtonInputHandler additionalButtonInputHandler_;
    GridInputHandler gridInputHandler_;
    RotaryControlInputHandler rotaryControlInputHandler_;

};

} // namespace

#endif // APPLICATION_LAUNCHPAD_H_
