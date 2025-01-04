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
    explicit RotaryControls( hardware::grid::InputInterface* gridDriver );

    bool waitForInput( Event* event ) override;
    void discardPendingInput() override;

private:
    void Run() override;

    int8_t calculateVelocityMultiplier( uint32_t intervalMs );

    void copyInput();
    int8_t getRecentMicrosteps( uint8_t controlIndex ) const;

    hardware::grid::InputInterface& gridDriver_;
    freertos::Queue events_;
    freertos::BinarySemaphore changesAvailable_;
    etl::array<etl::array<uint8_t, hardware::grid::numberOfColumns>, NUMBER_OF_CONTROLS> input_;
};

}  // namespace rotary_controls
