#pragma once

#include <stdint.h>

// both Grid and AdditionalButtons classes use these types, that's why they're grobal
enum ButtonAction : uint8_t
{
    ButtonAction_RELEASED = 0,
    ButtonAction_PRESSED
};
