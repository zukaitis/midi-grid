#pragma once

#include "additional_buttons/AdditionalButtonsInterface.h"
#include <freertos/thread.hpp>
#include <freertos/queue.hpp>

#include <etl/array.h>

namespace hardware
{
    namespace grid
    {
        class InputInterface;
    }
}

namespace additional_buttons
{

typedef etl::array<bool, NUMBER_OF_BUTTONS> InputArray;

// class used to acquire values from two additional buttons and rotary encoders
class AdditionalButtons : private freertos::Thread, public AdditionalButtonsInterface
{
public:
    explicit AdditionalButtons( hardware::grid::InputInterface& gridDriver );

    bool waitForInput( Event& event ) override;
    void discardPendingInput() override;

private:
    void Run() override;

    void copyInput();

    hardware::grid::InputInterface& gridDriver_;
    freertos::Queue events_;

    etl::array<InputArray, 2> input_;
    InputArray registeredInput_;
};

}  // namespace additional_buttons
