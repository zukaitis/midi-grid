#pragma once

#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>
#include <freertos/queue.hpp>

#include "io/additional_buttons/AdditionalButtonsInterface.h"
#include "io/grid/GridInterface.h"
#include "io/rotary_controls/RotaryControlsInterface.h"
#include "io/usb/UsbMidi.hpp"

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

    void delay( const uint32_t periodMs );

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
    virtual void handleAdditionalButtonEvent( const additional_buttons::Event event );
    virtual void handleGridButtonEvent( const grid::ButtonEvent event );
    virtual void handleRotaryControlEvent( const rotary_controls::Event event );
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
    ApplicationController( additional_buttons::AdditionalButtonsInterface& additionalButtons, grid::GridInterface& grid, rotary_controls::RotaryControlsInterface& rotaryControls,
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
    void handleInput( const additional_buttons::Event event );
    void handleInput( const grid::ButtonEvent event );
    void handleInput( const rotary_controls::Event event );
    void handleInput( const midi::MidiPacket packet );
    void runApplicationThread( ApplicationThread& thread );

private:
    void Run();

    Application* application_[kNumberOfApplications];
    Application* currentlyOpenApplication_;
    freertos::Queue nextApplication_;
    static bool applicationFinished_;

    InputHandler<additional_buttons::AdditionalButtonsInterface&, additional_buttons::Event> additionalButtonInputHandler_;
    InputHandler<grid::GridInterface&, grid::ButtonEvent> gridInputHandler_;
    InputHandler<rotary_controls::RotaryControlsInterface&, rotary_controls::Event> rotaryControlInputHandler_;
    InputHandler<midi::UsbMidi&, bool> midiInputAvailableHandler_;
    InputHandler<midi::UsbMidi&, midi::MidiPacket> midiInputHandler_;
    ApplicationThread applicationThread_;
};

} // namespace
