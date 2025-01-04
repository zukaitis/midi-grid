#pragma once

#include "io/HardwareInputInterface.h" // TODO: replace this nonsense name
#include <cstdint>

namespace rotary_controls
{

const uint8_t NUMBER_OF_CONTROLS = 2;

struct Event
{
    int8_t steps;
    uint8_t control;
};

class RotaryControlsInterface : public HardwareInputInterface<Event>
{
public:
    bool waitForInput( Event* event ) override = 0;
    void discardPendingInput() override = 0;
};

}  // namespace rotary_controls
