#include "application/Application.h"

namespace application
{

template <class InputSource, class InputType>
InputSource const InputHandler<InputSource, InputType>::handlerUnused = NULL;

template <class InputSource, class InputType>
InputHandler<InputSource, InputType>::InputHandler( Application& application ):
    Thread( "InputHandler", 200, 3 ),
    application_( application ),
    inputSource_( handlerUnused )
{
    Start();
    Suspend();
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::initialize( InputSource inputSource )
{
    inputSource_ = inputSource;
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::enable()
{
    if (handlerUnused != usbMidi_)
    {
        Resume();
    }
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::disable()
{
    if (handlerUnused != usbMidi_)
    {
        Suspend();
    }
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::Run()
{
    while (true)
    {
        InputType input = {};

        if (inputSource_->waitForInput( input ))
        {
            application_.handleInput( input );
        }
    }
}


Application::Application( ApplicationController& applicationController ):
    additionalButtonInputHandler_( InputHandler<grid::AdditionalButtons*, grid::AdditionalButtons::Event>( *this ) ),
    gridInputHandler_( InputHandler<grid::Grid*, grid::Grid::ButtonEvent>( *this ) ),
    rotaryControlInputHandler_( InputHandler<grid::RotaryControls*, grid::RotaryControls::Event>( *this ) ),
    midiInputAvailableHandler_( InputHandler<midi::UsbMidi*, void>( *this ) ),
    midiInputHandler_( InputHandler<midi::UsbMidi*, midi::MidiPacket>( *this ) ),
    applicationController_( applicationController )
{
}

void Application::open()
{
    initialize();

    additionalButtonInputHandler_.enable();
    gridInputHandler_.enable();
    rotaryControlInputHandler_.enable();
    midiInputAvailableHandler_.enable();
    midiInputHandler_.enable();
}

void Application::close()
{
    deinitialize();

    additionalButtonInputHandler_.disable();
    gridInputHandler_.disable();
    rotaryControlInputHandler_.disable();
    midiInputAvailableHandler_.disable();
    midiInputHandler_.disable();
}

void Application::switchApplication( const ApplicationIndex applicationIndex )
{
    close();
    applicationController_.selectApplication( applicationIndex );
}

void Application::initializeAdditionalButtonInputHandler( grid::AdditionalButtons& additionalButtons )
{
    additionalButtonInputHandler_.initialize( &additionalButtons );
}

void Application::initializeGridInputHandler( grid::Grid& grid )
{
    gridInputHandler_.initialize( &grid );
}

void Application::initializeRotaryControlInputHandler( grid::RotaryControls& rotaryControls )
{
    rotaryControlInputHandler_.initialize( &rotaryControls );
}

void Application::initializeMidiInputAvailableHandler( midi::UsbMidi& usbMidi )
{
    midiInputAvailableHandler_.initialize( &usbMidi );
}

void Application::initializeMidiInputHandler( midi::UsbMidi& usbMidi )
{
    midiInputHandler_.initialize( &usbMidi );
}

void Application::handleInput()
{
    handleMidiPacketAvailable();
}

void Application::handleInput( const grid::AdditionalButtons::Event event )
{
    handleAdditionalButtonEvent( event );
}

void Application::handleInput( const grid::Grid::ButtonEvent event )
{
    handleGridButtonEvent( event );
}

void Application::handleInput( const grid::RotaryControls::Event event )
{
    handleRotaryControlEvent( event );
}

void Application::handleInput( const midi::MidiPacket packet )
{
    handleMidiPacket( packet );
}

void Application::initialize()
{
    // do nothing by default, this method is to be overridden
}

void Application::deinitialize()
{
    // do nothing by default, this method is to be overridden
}

void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event )
{
    // do nothing by default, this method is to be overridden
}

void handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    // do nothing by default, this method is to be overridden
}

void handleRotaryControlEvent( const grid::RotaryControls::Event event )
{
    // do nothing by default, this method is to be overridden
}

void handleMidiPacketAvailable()
{
    // do nothing by default, this method is to be overridden
}

void handleMidiPacket( const midi::MidiPacket packet )
{
    // do nothing by default, this method is to be overridden
}

ApplicationController::ApplicationController( Application** const applicationList ):
    Thread( "ApplicationController", 200, 4 ),
    currentlyOpenApplication_( NULL )
{
    for (uint8_t index = 0; index < kNumberOfApplications; index++)
    {
        application_[index] = applicationList[index];
    }

    currentlyOpenApplication_ = application_[ApplicationIndex_STARTUP];
    Start();
}

void ApplicationController::selectApplication( const ApplicationIndex applicationIndex )
{
    Notify( static_cast<uint32_t>(applicationIndex) );
}

void ApplicationController::Run()
{
    while (true)
    {
        // block until notification
        const uint32_t applicationIndex = TakeNotification();

        if (applicationIndex < kNumberOfApplications)
        {
            Application* const applicationBeingClosed = currentlyOpenApplication_;
            currentlyOpenApplication_ = application_[applicationIndex];
            application_[ApplicationIndex_PREVIOUS] = applicationBeingClosed;

            currentlyOpenApplication_->open();
        }
    }
}

}
