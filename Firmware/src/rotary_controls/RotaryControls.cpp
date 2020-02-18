#include "rotary_controls/RotaryControls.h"

#include "ThreadConfigurations.h"
#include <freertos/ticks.hpp>
#include <cstdlib>

namespace rotary_controls
{

static const int8_t kNumberOfMicrostepsInStep = 4;
static const etl::array<int8_t, 16> kEncoderState = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0 };
static const etl::array<uint32_t, NUMBER_OF_CONTROLS> kEncoderBitShift = {14, 11};
static const uint32_t kEncoderMask = 0x03; // two LSBs

RotaryControls::RotaryControls( hardware::grid::InputInterface* gridDriver ) :
    Thread( "RotaryControls", kRotaryControls.stackDepth, kRotaryControls.priority ),
    gridDriver_( *gridDriver ),
    events_( 8, sizeof( Event ) ),
    input_()
{
    //gridDriver_.addThreadToNotify( this );
    gridDriver_.addSemaphoreToGive( &changesAvailable_ );
    Start();
}

bool RotaryControls::waitForInput( Event* event )
{
    const bool eventAvailable = events_.Dequeue( event ); // block until event
    return eventAvailable;
}

void RotaryControls::discardPendingInput()
{
    events_.Flush();
}

void RotaryControls::Run()
{
    static etl::array<int8_t, NUMBER_OF_CONTROLS> microsteps = {};
    static etl::array<TickType_t, NUMBER_OF_CONTROLS> previousEventTime = {};

    // Thread::WaitForNotification();
    changesAvailable_.Take();

    copyInput();

    for (uint8_t controlIndex = 0; controlIndex < NUMBER_OF_CONTROLS; controlIndex++)
    {
        microsteps.at( controlIndex ) += getRecentMicrosteps( controlIndex );

        if (std::abs( microsteps.at( controlIndex ) ) >= kNumberOfMicrostepsInStep)
        {
            const uint32_t interval = freertos::Ticks::TicksToMs( freertos::Ticks::GetTicks() - previousEventTime.at( controlIndex ) );
            previousEventTime.at( controlIndex ) = freertos::Ticks::GetTicks();
            const int8_t velocityMultiplier = calculateVelocityMultiplier( interval );

            Event event = {
                .steps = static_cast<int8_t>((microsteps.at( controlIndex ) / kNumberOfMicrostepsInStep) * velocityMultiplier),
                .control = controlIndex };
            microsteps.at( controlIndex ) %= kNumberOfMicrostepsInStep;

            if (false == events_.IsFull())
            {
                events_.Enqueue( &event );
            }
        }
    }
}

int8_t RotaryControls::calculateVelocityMultiplier( const uint32_t intervalMs )
{
    int8_t multiplier = 1;
    
    if (intervalMs > 500)
    {
        multiplier = 1;
    }
    else if (intervalMs > 200)
    {
        multiplier = 2;
    }
    else if (intervalMs > 50)
    {
        multiplier = 4;
    }
    else
    {
        multiplier = 8;
    }

    return multiplier;
}

void RotaryControls::copyInput()
{
    const hardware::grid::InputBuffer& hardwareInput = gridDriver_.getStableInputBuffer();

    for (uint8_t controlIndex = 0; controlIndex < NUMBER_OF_CONTROLS; controlIndex++)
    {
        for (uint32_t i = 0; i < input_.at( controlIndex ).size(); i++)
        {
            input_.at( controlIndex ).at( i ) = (hardwareInput.at( i ) >> kEncoderBitShift.at( controlIndex )) & kEncoderMask;
        }
    }
}

int8_t RotaryControls::getRecentMicrosteps( const uint8_t controlIndex ) const
{
    static etl::array<uint8_t, NUMBER_OF_CONTROLS> recentEncoderValues = {};
    int8_t microsteps = 0;

    for (const uint8_t& inputValue : input_.at( controlIndex ))
    {
        recentEncoderValues.at( controlIndex ) <<= 2U;
        recentEncoderValues.at( controlIndex ) |= inputValue;
        recentEncoderValues.at( controlIndex ) &= 0x0FU;
        const uint8_t stateIndex = recentEncoderValues.at( controlIndex );
        microsteps += kEncoderState.at( stateIndex );
    }

    return microsteps;
}

}  // namespace rotary_controls
