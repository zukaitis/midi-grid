#ifndef THREAD_CONFIGURATIONS_H_
#define THREAD_CONFIGURATIONS_H_

#include <stdint.h>

struct ThreadConfiguration
{
    uint16_t stackDepth;
    uint8_t priority;
};

static const ThreadConfiguration kInputSource = { .stackDepth = 512, .priority = 3 };
static const ThreadConfiguration kApplicationController = { .stackDepth = 2048, .priority = 4 };
static const ThreadConfiguration kApplicationThread = { .stackDepth = 512, .priority = 3 };
static const ThreadConfiguration kAdditionalButtons = { .stackDepth = 256, .priority = 5 };
static const ThreadConfiguration kGrid = { .stackDepth = 256, .priority = 5 };
static const ThreadConfiguration kRotaryControls = { .stackDepth = 128, .priority = 5 };
static const ThreadConfiguration kFlashingLeds = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kPulsingLeds = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kLcd = { .stackDepth = 256, .priority = 2 };
static const ThreadConfiguration kGui = { .stackDepth = 128, .priority = 2 };

#endif // THREAD_CONFIGURATIONS_H_