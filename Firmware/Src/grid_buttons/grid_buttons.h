/*
 * grid_buttons.h
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "stm32f4xx_hal.h"

enum ButtonEventType
{
    ButtonEvent_RELEASED = 0,
    ButtonEvent_PRESSED
};

enum LedLightingType
{
    LedLighting_LIGHT = 0,
    LedLighting_FLASH,
    LedLighting_PULSE
};

struct Colour
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

void grid_enable();
uint8_t grid_getButtonEvent(uint8_t* buttonPositionX, uint8_t* buttonPositionY, uint8_t* buttonEvent);
void grid_initialize();
void grid_setLedColourFromLaunchpadPalette( uint8_t ledPositionX, uint8_t ledPositionY, uint8_t colourNumber );
void grid_setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const struct Colour* colour );
void grid_setLedFromMidiMessage(uint8_t ledPositionX, uint8_t ledPositionY, uint8_t colourCode, uint8_t controlType);
void grid_setLedOutputDirectly( uint8_t ledPositionX, uint8_t ledPositionY, uint16_t outputRed, uint16_t outputGreen,
        uint16_t outputBlue );
void grid_updateLeds();


#endif /* GRID_BUTTONS_H_ */
