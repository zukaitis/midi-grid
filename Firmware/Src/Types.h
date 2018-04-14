/*
 * Types.h
 *
 *  Created on: 2018-03-10
 *      Author: Gedas
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

struct Colour
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

enum ButtonEvent
{
    ButtonEvent_RELEASED = 0,
    ButtonEvent_PRESSED
};

#endif /* TYPES_H_ */
