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

struct FlashingLed
{
    uint8_t positionX;
    uint8_t positionY;
    Colour alternateColour;
};

struct PulsingLed
{
    uint8_t positionX;
    uint8_t positionY;
};

struct GridLed
{
    Colour colour;
    LedLightingType lightingType; // light?flash?pulse
};

static const uint8_t NUMBER_OF_ROWS = 8;
static const uint8_t NUMBER_OF_COLUMNS = 10;

static const uint32_t LED_FLASH_PERIOD_MS = 250; // 120bpm - default flashing rate
static const uint32_t LED_PULSE_STEP_PERIOD_MS = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t LED_PULSE_STEP_COUNT = 15;

class Grid
{
public:
    void enable();
    bool getButtonEvent(uint8_t* buttonPositionX, uint8_t* buttonPositionY, ButtonEvent* buttonEvent);
    void initialize();
    void refreshLeds();

    void setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour);
    void setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType);
    void turnAllLedsOff();

    bool areColoursEqual(const Colour& colour1, const Colour& colour2);
private:
    void setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour );

    grid_control::GridControl& gridControl;
    bool initialized = false;

};

} //namespace
#endif /* GRID_BUTTONS_H_ */
