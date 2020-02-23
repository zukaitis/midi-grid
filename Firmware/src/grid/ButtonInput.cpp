#include "grid/ButtonInput.h"

#include "grid/GridInterface.h"
#include "hardware/grid/InputInterface.h"
#include "system/GlobalInterruptsInterface.h"
#include "ThreadConfigurations.h"
#include "types/ButtonAction.h"

#include <algorithm>
#include <etl/array.h>

namespace grid
{

static const uint16_t INPUT_MASK = 0x000F;

ButtonInput::ButtonInput( hardware::grid::InputInterface* gridDriver, mcu::GlobalInterruptsInterface* globalInterrupts ):
    Thread( "grid::ButtonInput", kGrid.stackDepth, kGrid.priority ),
    globalInterrupts_( *globalInterrupts ),
    gridDriver_( *gridDriver ),
    events_( freertos::Queue( 16, sizeof( ButtonEvent )))
{
    // gridDriver_.addThreadToNotify( this );
    gridDriver_.addSemaphoreToGive( &changesAvailable_ );
    Thread::Start();
}

bool ButtonInput::waitForEvent( ButtonEvent* event )
{
    const bool eventAvailable = events_.Dequeue( event ); // block until event
    return eventAvailable;
}

void ButtonInput::discardPendingEvents()
{
    events_.Flush();
}

void ButtonInput::Run()
{
    static const InputBuffer& inputBuffer = inputBuffers_[0];

    // Thread::WaitForNotification(); // blocking until grid driver gives notification
    changesAvailable_.Take();

    globalInterrupts_.disable();
    copyInputBuffers();
    globalInterrupts_.enable();

    InputBuffer debouncingBuffer;
    fillDebouncingBuffer( debouncingBuffer );
    InputBuffer changesBuffer;
    fillChangesBuffer( changesBuffer );

    for (uint8_t x = 0; x < inputBuffer.size(); x++)
    {
        if ((0 == debouncingBuffer[x]) && (0 != changesBuffer[x]))
        {
            for (uint8_t y = 0; y < hardware::grid::numberOfRows; y++)
            {
                if (0 != ((changesBuffer[x] >> y) & 0x01))
                {
                    ButtonEvent event = {
                        .action = static_cast<ButtonAction>((inputBuffer[x] >> y) & 0x01),
                        .coordinates = calculatePhysicalCoordinates({ x, y }) };

                    registeredInputBuffer_[x] ^= (1U << y); // toggle bit that was registered

                    if (false == events_.IsFull())
                    {
                        events_.Enqueue( &event );
                    }
                }
            }
        }
    }
}

void ButtonInput::copyInputBuffers()
{
    inputBuffers_ = gridDriver_.getInputDebouncingBuffers();
}

void ButtonInput::fillDebouncingBuffer( InputBuffer& debouncingBuffer ) const
{
    for (uint8_t i = 0; i < debouncingBuffer.size(); i++)
    {
        debouncingBuffer[i] = inputBuffers_[0][i] ^ inputBuffers_[1][i];
        debouncingBuffer[i] &= INPUT_MASK;
    }

#if 0 // fancy implementation, for the future
    std::transform( inputBuffer[0].begin(), inputBuffer[0].end(),
        inputBuffer[1].begin(),
        debouncingBuffer.begin(),
        std::bit_xor<uint32_t>() );

    std::transform( debouncingBuffer.begin(), debouncingBuffer.end(),
        0xFF,
        debouncingBuffer.begin(),
        std::bit_xor<uint32_t>() );
#endif
}

void ButtonInput::fillChangesBuffer( InputBuffer& changesBuffer ) const
{
    const InputBuffer& inputBuffer = inputBuffers_[0];

    for (uint8_t i = 0; i < inputBuffer.size(); i++)
    {
        changesBuffer[i] = inputBuffer[i] ^ registeredInputBuffer_[i];
        changesBuffer[i] &= INPUT_MASK;
    }
}

Coordinates ButtonInput::calculatePhysicalCoordinates( const Coordinates& hardwareCoordinates ) const
{
    Coordinates physicalCoordinates = hardwareCoordinates;

    if (hardwareCoordinates.x >= numberOfColumns)
    {
        physicalCoordinates.x -= numberOfColumns;
        physicalCoordinates.y += hardware::grid::numberOfRows;
    }

    return physicalCoordinates;
}

}
