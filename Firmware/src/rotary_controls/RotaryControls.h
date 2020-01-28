#pragma once

#include "rotary_controls/RotaryControlsInterface.h"
#include "hardware/grid/InputInterface.h"

#include <freertos/thread.hpp>
#include <freertos/queue.hpp>

#include <etl/array.h>

namespace rotary_controls
{

class RotaryControls : private freertos::Thread, public RotaryControlsInterface
{
public:
    RotaryControls( hardware::grid::InputInterface& gridDriver );

    bool waitForInput( Event& event ) override;
    void discardPendingInput() override;

private:
    void Run() override;

    int8_t calculateVelocityMultiplier( uint32_t intervalMs ) const;

    void copyInput();
    int8_t getRecentMicrosteps( uint8_t controlIndex ) const;

    hardware::grid::InputInterface& gridDriver_;
    freertos::Queue events_;
    etl::array<etl::array<uint8_t, hardware::grid::numberOfColumns>, NUMBER_OF_CONTROLS> input_;
};

}
