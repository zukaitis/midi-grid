#pragma once

#include <cstdint>

struct ThreadConfiguration
{
    uint16_t stackDepth;
    uint8_t priority;
};

const ThreadConfiguration kAdditionalButtons = { .stackDepth = 256, .priority = 7 };
const ThreadConfiguration kGrid = { .stackDepth = 256, .priority = 7 };
const ThreadConfiguration kRotaryControls = { .stackDepth = 128, .priority = 7 };
const ThreadConfiguration kApplicationController = { .stackDepth = 1024, .priority = 5 };
const ThreadConfiguration kInputSource = { .stackDepth = 512, .priority = 3 };
const ThreadConfiguration kApplicationThread = { .stackDepth = 512, .priority = 3 };
const ThreadConfiguration kFlashingLeds = { .stackDepth = 256, .priority = 2 };
const ThreadConfiguration kPulsingLeds = { .stackDepth = 256, .priority = 2 };
const ThreadConfiguration kLcd = { .stackDepth = 256, .priority = 2 };
const ThreadConfiguration kBacklight = { .stackDepth = 256, .priority = 2 };
