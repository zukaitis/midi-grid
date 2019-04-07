#include "grid/AdditionalButtons.h"
#include "grid/GridDriver.h"
#include "ticks.hpp"

#include <stdlib.h>

namespace grid
{

static const uint8_t kInputEventQueueSize = 2;

AdditionalButtons::AdditionalButtons( GridDriver& gridDriver ) :
        Thread( "AdditionalButtons", 200, 4 ),
        gridDriver_( gridDriver ),
        inputEvents_( freertos::Queue( kInputEventQueueSize, sizeof( Event ) ) )
{
    // active low
    registeredButtonInput_[0] = true;
    registeredButtonInput_[1] = true;

    gridDriver_.addNotificationCallback( std::bind( &AdditionalButtons::notifyFromISRWrapper, this ) );
    Start();
}

AdditionalButtons::~AdditionalButtons()
{
}

void AdditionalButtons::discardAllPendingEvents()
{
    inputEvents_.Flush();
}

bool AdditionalButtons::getEvent( Event& event )
{
    const bool eventAvailable = inputEvents_.Dequeue( &event, 1 );
    return eventAvailable;
}

void AdditionalButtons::Run()
{
    while (true)
    {
        TakeNotification(); // blocking until grid driver gives notification

        for (uint8_t buttonIndex = 0; buttonIndex < numberOfButtons; buttonIndex++)
        {
            if (gridDriver_.isButtonInputStable( buttonIndex ))
            {
                const bool buttonInput = gridDriver_.getButtonInput(buttonIndex);
                if (registeredButtonInput_[buttonIndex] != buttonInput)
                {
                    Event event = {};
                    event.action = buttonInput ? ButtonAction_RELEASED : ButtonAction_PRESSED; // active low
                    event.button = static_cast<Button>(buttonIndex);
                    registeredButtonInput_[buttonIndex] = buttonInput;
                    inputEvents_.Enqueue( &event );
                }
            }
        }
    }
}

} // namespace grid
