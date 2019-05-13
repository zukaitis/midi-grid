#include "application/Application.h"

namespace application
{

template <class InputSource, class InputType>
InputHandler<InputSource, InputType>::InputHandler( ApplicationController& applicationController, InputSource inputSource ):
    Thread( "InputHandler", 500, 3 ),
    applicationController_( applicationController ),
    inputSource_( inputSource )
{
    Start();
    Suspend();
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::enable()
{
    Resume();
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::disable()
{
    Suspend();
}

template <class InputSource, class InputType>
void InputHandler<InputSource, InputType>::Run()
{
    while (true)
    {
        InputType input = {};

        if (inputSource_.waitForInput( input ))
        {
            applicationController_.handleInput( input );
        }
    }
}

ApplicationThread::ApplicationThread( ApplicationController& applicationController ):
    Thread( "ApplicationThread", 500, 3 ),
    applicationController_( applicationController )
{
    Start();
    Suspend();
}

void ApplicationThread::enable()
{
    Resume();
}

void ApplicationThread::disable()
{
    Suspend();
}

void ApplicationThread::Run()
{
    applicationController_.runApplicationThread( *this );
}

Application::Application( ApplicationController& applicationController ):
    applicationController_( applicationController )
{
}

Application::~Application()
{
}

void Application::switchApplication( const ApplicationIndex applicationIndex )
{
    applicationController_.selectApplication( applicationIndex );
}

void Application::enableAdditionalButtonInputHandler()
{
    applicationController_.enableAdditionalButtonInputHandler();
}

void Application::enableGridInputHandler()
{
    applicationController_.enableGridInputHandler();
}

void Application::enableRotaryControlInputHandler()
{
    applicationController_.enableRotaryControlInputHandler();
}

void Application::enableMidiInputAvailableHandler()
{
    applicationController_.enableMidiInputAvailableHandler();
}

void Application::enableMidiInputHandler()
{
    applicationController_.enableMidiInputHandler();
}

void Application::run( ApplicationThread& thread )
{
    // do nothing by default, this method is to be overridden
}

void Application::handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event )
{
    // do nothing by default, this method is to be overridden
}

void Application::handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    // do nothing by default, this method is to be overridden
}

void Application::handleRotaryControlEvent( const grid::RotaryControls::Event event )
{
    // do nothing by default, this method is to be overridden
}

void Application::handleMidiPacketAvailable()
{
    // do nothing by default, this method is to be overridden
}

void Application::handleMidiPacket( const midi::MidiPacket packet )
{
    // do nothing by default, this method is to be overridden
}

ApplicationController::ApplicationController( grid::AdditionalButtons& additionalButtons, grid::Grid& grid,
    grid::RotaryControls& rotaryControls, midi::UsbMidi& usbMidi ):
        Thread( "ApplicationController", 500, 4 ),
        currentlyOpenApplication_( NULL ),
        nextApplication_( freertos::Queue( 2, sizeof( ApplicationIndex ) ) ),
        additionalButtonInputHandler_( InputHandler<grid::AdditionalButtons&, grid::AdditionalButtons::Event>( *this, additionalButtons ) ),
        gridInputHandler_( InputHandler<grid::Grid&, grid::Grid::ButtonEvent>( *this, grid ) ),
        rotaryControlInputHandler_( InputHandler<grid::RotaryControls&, grid::RotaryControls::Event>( *this, rotaryControls ) ),
        midiInputAvailableHandler_( InputHandler<midi::UsbMidi&, bool>( *this, usbMidi ) ),
        midiInputHandler_( InputHandler<midi::UsbMidi&, midi::MidiPacket>( *this, usbMidi ) ),
        applicationThread_( ApplicationThread( *this ) )
{
}

void ApplicationController::initialize( Application** const applicationList )
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
    disableAllHandlers();
    applicationThread_.disable();
    //Notify( static_cast<uint32_t>(applicationIndex) );
    ApplicationIndex index = applicationIndex;
    nextApplication_.Enqueue( &index );
}

void ApplicationController::Run()
{
    nextApplication_.Flush();
    // open Startup application instantly
    currentlyOpenApplication_ = application_[ApplicationIndex_STARTUP];
    applicationThread_.enable();

    while (true)
    {
        // block until notification
        /*const*/ ApplicationIndex applicationIndex = {}; //WaitForNotification();
        nextApplication_.Dequeue( &applicationIndex );

        if (applicationIndex < kNumberOfApplications)
        {
            Application* const applicationBeingClosed = currentlyOpenApplication_;
            currentlyOpenApplication_ = application_[applicationIndex];
            application_[ApplicationIndex_PREVIOUS] = applicationBeingClosed;

            applicationThread_.enable();
        }
    }
}

void ApplicationController::enableAdditionalButtonInputHandler()
{
    additionalButtonInputHandler_.enable();
}

void ApplicationController::enableGridInputHandler()
{
    gridInputHandler_.enable();
}

void ApplicationController::enableRotaryControlInputHandler()
{
    rotaryControlInputHandler_.enable();
}

void ApplicationController::enableMidiInputAvailableHandler()
{
    midiInputAvailableHandler_.enable();
}

void ApplicationController::enableMidiInputHandler()
{
    midiInputHandler_.enable();
}

void ApplicationController::disableAllHandlers()
{
    additionalButtonInputHandler_.disable();
    gridInputHandler_.disable();
    rotaryControlInputHandler_.disable();
    midiInputAvailableHandler_.disable();
    midiInputHandler_.disable();
}

void ApplicationController::handleInput( const bool dummy )
{
    currentlyOpenApplication_->handleMidiPacketAvailable();
}

void ApplicationController::handleInput( const grid::AdditionalButtons::Event event )
{
    currentlyOpenApplication_->handleAdditionalButtonEvent( event );
}

void ApplicationController::handleInput( const grid::Grid::ButtonEvent event )
{
    currentlyOpenApplication_->handleGridButtonEvent( event );
}

void ApplicationController::handleInput( const grid::RotaryControls::Event event )
{
    currentlyOpenApplication_->handleRotaryControlEvent( event );
}

void ApplicationController::handleInput( const midi::MidiPacket packet )
{
    currentlyOpenApplication_->handleMidiPacket( packet );
}

void ApplicationController::runApplicationThread( ApplicationThread& thread )
{
    while(true)
    {
        currentlyOpenApplication_->run( thread );
        applicationThread_.disable();
    }
}

}
