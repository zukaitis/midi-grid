#pragma once

#include <cstdint>

struct ThreadConfiguration
{
    uint16_t stackDepth;
    uint8_t priority;
};

static const ThreadConfiguration kAdditionalButtons = { .stackDepth = 256, .priority = 7 };
static const ThreadConfiguration kGrid = { .stackDepth = 256, .priority = 7 };
static const ThreadConfiguration kRotaryControls = { .stackDepth = 128, .priority = 7 };
static const ThreadConfiguration kApplicationController = { .stackDepth = 1024, .priority = 5 };
static const ThreadConfiguration kInputSource = { .stackDepth = 512, .priority = 3 };
static const ThreadConfiguration kApplicationThread = { .stackDepth = 512, .priority = 3 };
static const ThreadConfiguration kFlashingLeds = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kPulsingLeds = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kLcd = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kBacklight = { .stackDepth = 256, .priority = 2 };
