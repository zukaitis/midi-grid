/*
 * grid_buttons.h
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#ifndef GRID_BUTTONS_H_
#define GRID_BUTTONS_H_

#include "stm32f4xx_hal.h"

namespace grid
{

enum ButtonEvent
{
    ButtonEvent_RELEASED = 0,
    ButtonEvent_PRESSED
};

enum LedLightingType
{
    LedLightingType_LIGHT = 0,
    LedLightingType_FLASH,
    LedLightingType_PULSE
};

struct Colour
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

#define NUMBER_OF_ROWS      8
#define NUMBER_OF_COLUMNS   10

void grid_setAllLedsOff();
void grid_setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const struct Colour* colour );
void grid_setLed(uint8_t ledPositionX, uint8_t ledPositionY, const struct Colour* colour, enum LedLightingType ledLightingType);
void grid_refreshLeds();
uint8_t grid_areColoursEqual(const struct Colour * colour1, const struct Colour * colour2);

struct Colour grid_getLedColour(uint8_t ledPositionX, uint8_t ledPositionY);

class Grid
{
public:

    void enable();
    bool getButtonEvent(uint8_t* buttonPositionX, uint8_t* buttonPositionY, ButtonEvent* buttonEvent);
    void initialize();

    void setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour* colour);
    void setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour* colour, const LedLightingType lightingType);

private:
    grid_control::GridControl& gridControl;
    bool initialized = false;

};


} //namespace
#endif /* GRID_BUTTONS_H_ */
