#include "io/rotary_controls/RotaryControls.h"

#include "ThreadConfigurations.h"
#include <freertos/ticks.hpp>
#include <cstdlib>

namespace rotary_controls
{

static const int8_t kNumberOfMicrostepsInStep = 4;
static const int8_t kEncoderState[16] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0,-1, 1, 0 };
static const etl::array<uint32_t, NUMBER_OF_CONTROLS> kEncoderBitShift = {14, 11};
static const uint32_t kEncoderMask = 0x03; // two LSBs

RotaryControls::RotaryControls( hardware::grid::InputInterface& gridDriver ) :
        Thread( "RotaryControls", kRotaryControls.stackDepth, kRotaryControls.priority ),
        gridDriver_( gridDriver ),
        events_( freertos::Queue( 8, sizeof( Event ) ) )
{
    gridDriver_.addThreadToNotify( this );
    Start();
}

bool RotaryControls::waitForInput( Event& event )
{
    const bool eventAvailable = events_.Dequeue( &event ); // block until event
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

    while (true)
    {
        Thread::WaitForNotification();

        copyInput();

        for (uint8_t controlIndex = 0; controlIndex < NUMBER_OF_CONTROLS; controlIndex++)
        {
            microsteps[controlIndex] += getRecentMicrosteps( controlIndex );

            if (std::abs( microsteps[controlIndex] ) >= kNumberOfMicrostepsInStep)
            {
                const uint32_t interval = freertos::Ticks::TicksToMs( freertos::Ticks::GetTicks() - previousEventTime[controlIndex] );
                previousEventTime[controlIndex] = freertos::Ticks::GetTicks();
                const int8_t velocityMultiplier = calculateVelocityMultiplier( interval );

                Event event = {
                    .steps = static_cast<int8_t>((microsteps[controlIndex] / kNumberOfMicrostepsInStep) * velocityMultiplier),
                    .control = controlIndex };
                microsteps[controlIndex] %= kNumberOfMicrostepsInStep;

                events_.Enqueue( &event );
            }
        }
    }
}

int8_t RotaryControls::calculateVelocityMultiplier( const uint32_t intervalMs ) const
{
    int8_t multiplier = 0;
    
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
        for (uint8_t i = 0; i < input_[controlIndex].size(); i++)
        {
            input_[controlIndex][i] = (hardwareInput[i] >> kEncoderBitShift[controlIndex]) & kEncoderMask;
        }
    }
}

int8_t RotaryControls::getRecentMicrosteps( const uint8_t controlIndex ) const
{
    static etl::array<uint8_t, NUMBER_OF_CONTROLS> recentEncoderValues = {};
    int8_t microsteps = 0;

    for (const uint8_t& inputValue : input_[controlIndex])
    {
        recentEncoderValues[controlIndex] <<= 2;
        recentEncoderValues[controlIndex] |= inputValue;
        recentEncoderValues[controlIndex] &= 0x0F;
        const uint8_t stateIndex = recentEncoderValues[controlIndex];
        microsteps += kEncoderState[stateIndex];
    }

    return microsteps;
}

}
