#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "thread.hpp"
#include "semaphore.hpp"

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
    ApplicationIndex_SNAKE,
    kNumberOfApplications
};

class Application;
class ApplicationController;

template <class InputSource, class InputType>
class InputHandler : private freertos::Thread
{
public:
    InputHandler( ApplicationController& applicationController, InputSource inputSource );

    void enable();
    void disable();

    void Run();

private:
    ApplicationController& applicationController_;
    InputSource inputSource_;
};

class ApplicationThread : public freertos::Thread
{
public:
    ApplicationThread( ApplicationController& applicationController );

    void enable();
    void disable();
    void run();

private:
    void Run();

    ApplicationController& applicationController_;
    freertos::BinarySemaphore continueApplication_;
};

class Application
{
public:
    Application( ApplicationController& applicationController );
    virtual ~Application();

    virtual void run( ApplicationThread& thread );
    virtual void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event );
    virtual void handleGridButtonEvent( const grid::Grid::ButtonEvent event );
    virtual void handleRotaryControlEvent( const grid::RotaryControls::Event event );
    virtual void handleMidiPacket( const midi::MidiPacket packet );
    virtual void handleMidiPacketAvailable();

protected:
    void switchApplication( const ApplicationIndex application );

    void enableAdditionalButtonInputHandler();
    void enableGridInputHandler();
    void enableRotaryControlInputHandler();
    void enableMidiInputAvailableHandler();
    void enableMidiInputHandler();

private:
    ApplicationController& applicationController_;
};

class ApplicationController : private freertos::Thread
{
public:
    ApplicationController( grid::AdditionalButtons& additionalButtons, grid::Grid& grid, grid::RotaryControls& rotaryControls,
        midi::UsbMidi& usbMidi );

    void initialize( Application** const applicationList );

    void selectApplication( const ApplicationIndex applicationIndex );

    void enableAdditionalButtonInputHandler();
    void enableGridInputHandler();
    void enableRotaryControlInputHandler();
    void enableMidiInputAvailableHandler();
    void enableMidiInputHandler();
    void disableAllHandlers();

    void handleInput( const bool dummy );
    void handleInput( const grid::AdditionalButtons::Event event );
    void handleInput( const grid::Grid::ButtonEvent event );
    void handleInput( const grid::RotaryControls::Event event );
    void handleInput( const midi::MidiPacket packet );
    void runApplicationThread( ApplicationThread& thread );

private:
    void Run();

    Application* application_[kNumberOfApplications];
    Application* currentlyOpenApplication_;
    freertos::Queue nextApplication_;
    static bool applicationFinished_;

    InputHandler<grid::AdditionalButtons&, grid::AdditionalButtons::Event> additionalButtonInputHandler_;
    InputHandler<grid::Grid&, grid::Grid::ButtonEvent> gridInputHandler_;
    InputHandler<grid::RotaryControls&, grid::RotaryControls::Event> rotaryControlInputHandler_;
    InputHandler<midi::UsbMidi&, bool> midiInputAvailableHandler_;
    InputHandler<midi::UsbMidi&, midi::MidiPacket> midiInputHandler_;
    ApplicationThread applicationThread_;
};

} // namespace

#endif // APPLICATION_H_
