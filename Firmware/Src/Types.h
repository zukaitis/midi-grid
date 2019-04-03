#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

struct Color
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

// both Grid and AdditionalButtons classes use these types, that's why they're grobal
enum ButtonAction : uint8_t
{
    ButtonAction_RELEASED = 0,
    ButtonAction_PRESSED
};

struct ButtonEvent
{
    ButtonAction action;
    uint8_t positionX;
    uint8_t positionY;
};

#endif // TYPES_H_
