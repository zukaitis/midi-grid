#pragma once

#include <stdint.h>

// both Grid and AdditionalButtons classes use these types, that's why they're global
enum class ButtonAction : uint8_t
{
    RELEASED = 0,
    PRESSED
};
