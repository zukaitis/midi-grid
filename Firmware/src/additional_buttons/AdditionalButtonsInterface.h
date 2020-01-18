#pragma once

#include "HardwareInputInterface.h"
#include "types/ButtonAction.h"

namespace additional_buttons
{

enum class Button : uint8_t
{
    extraNote = 0,
    internalMenu = 1,
};

static const uint8_t NUMBER_OF_BUTTONS = 2;

struct Event
{
    ButtonAction action;
    Button button;
};

class AdditionalButtonsInterface : public HardwareInputInterface<Event>
{
public:
    virtual bool waitForInput( Event& event ) override = 0;
    virtual void discardPendingInput() override = 0;
};

}
