#include "io/additional_buttons/AdditionalButtons.h"
#include "hardware/grid/InputInterface.h"
#include <freertos/ticks.hpp>
#include "ThreadConfigurations.h"

namespace additional_buttons
{

static const etl::array<uint32_t, 2> BUTTON_MASK = {0x2000, 0x0400};

AdditionalButtons::AdditionalButtons( hardware::grid::InputInterface& gridDriver ) :
        Thread( "AdditionalButtons", kAdditionalButtons.stackDepth, kAdditionalButtons.priority ),
        gridDriver_( gridDriver ),
        events_( freertos::Queue( 3, sizeof( Event ) ) )
{
    registeredInput_.fill( false );

    gridDriver_.addThreadToNotify( this );
    Thread::Start();
}

void AdditionalButtons::discardPendingInput()
{
    events_.Flush();
}

bool AdditionalButtons::waitForInput( Event& event )
{
    const bool eventAvailable = events_.Dequeue( &event );
    return eventAvailable;
}

void AdditionalButtons::copyInput()
{
    const hardware::grid::InputDebouncingBuffers& inputBuffers = gridDriver_.getInputDebouncingBuffers();

    for (uint8_t buttonIndex = 0; buttonIndex < NUMBER_OF_BUTTONS; buttonIndex++)
    {
        // active low
        input_[0][buttonIndex] = (0 == (inputBuffers[0][0] & BUTTON_MASK[buttonIndex]));
        input_[1][buttonIndex] = (0 == (inputBuffers[1][0] & BUTTON_MASK[buttonIndex]));
    }
}

void AdditionalButtons::Run()
{
    while (true)
    {
        Thread::WaitForNotification(); // blocking until grid driver gives notification

        copyInput();

        for (uint8_t buttonIndex = 0; buttonIndex < NUMBER_OF_BUTTONS; buttonIndex++)
        {
            if (input_[0][buttonIndex] == input_[1][buttonIndex]) // debouncing
            {
                const bool input = input_[0][buttonIndex];
                if (input != registeredInput_[buttonIndex]) // checking for changes
                {
                    Event event = {
                        .action = input ? ButtonAction::PRESSED : ButtonAction::RELEASED,
                        .button = static_cast<Button>(buttonIndex) };
                    registeredInput_[buttonIndex] = input;
                    events_.Enqueue( &event );
                }
            }
        }
    }
}

}
