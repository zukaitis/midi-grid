#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "thread.hpp"

#include "grid/AdditionalButtons.h"
#include "grid/Grid.h"
#include "grid/RotaryControls.h"
#include "usb/UsbMidi.h"

namespace application
{

enum ApplicationIndex : uint8_t
{
    ApplicationIndex_PREVIOUS = 0,
    ApplicationIndex_STARTUP,
    ApplicationIndex_GRID_TEST,
    ApplicationIndex_INTERNAL_MENU,
    ApplicationIndex_LAUNCHPAD,
    kNumberOfApplications
};

class Application;
class ApplicationController;

template <class InputSource, class InputType>
class InputHandler : public freertos::Thread
{
public:
    InputHandler( Application& application );

    void initialize( InputSource inputSource );

    void enable();
    void disable();

    void Run();

private:
    static InputSource const handlerUnused;

    Application& application_;
    InputSource inputSource_;
};

class Application
{
public:
    Application( ApplicationController& applicationController );

    void open();
    void close();
    void switchApplication( const ApplicationIndex application );

    void initializeAdditionalButtonInputHandler( grid::AdditionalButtons& additionalButtons );
    void initializeGridInputHandler( grid::Grid& grid );
    void initializeRotaryControlInputHandler( grid::RotaryControls& rotaryControls );
    void initializeMidiInputAvailableHandler( midi::UsbMidi& usbMidi );
    void initializeMidiInputHandler( midi::UsbMidi& usbMidi );

    virtual void initialize();
    virtual void deinitialize();
    virtual void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event );
    virtual void handleGridButtonEvent( const grid::Grid::ButtonEvent event );
    virtual void handleRotaryControlEvent( const grid::RotaryControls::Event event );
    virtual void handleMidiPacket( const midi::MidiPacket packet );
    virtual void handleMidiPacketAvailable();

    void handleInput();
    void handleInput( const grid::AdditionalButtons::Event event );
    void handleInput( const grid::Grid::ButtonEvent event );
    void handleInput( const grid::RotaryControls::Event event );
    void handleInput( const midi::MidiPacket packet );

private:
    ApplicationController& applicationController_;

    //MidiInputHandler midiInputHandler_;
    InputHandler<grid::AdditionalButtons*, grid::AdditionalButtons::Event> additionalButtonInputHandler_;
    InputHandler<grid::Grid*, grid::Grid::ButtonEvent> gridInputHandler_;
    InputHandler<grid::RotaryControls*, grid::RotaryControls::Event> rotaryControlInputHandler_;
    InputHandler<midi::UsbMidi*, void /*bool*/> midiInputAvailableHandler_;
    InputHandler<midi::UsbMidi*, midi::MidiPacket> midiInputHandler_;
};

class ApplicationController : private freertos::Thread
{
public:
    ApplicationController( Application** const applicationList );

    void selectApplication( const ApplicationIndex applicationIndex );
protected:
    void Run();

private:
    Application* application_[kNumberOfApplications];
    Application* currentlyOpenApplication_;
};

} // namespace

#endif // APPLICATION_H_
