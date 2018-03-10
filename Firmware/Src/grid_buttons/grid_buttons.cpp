/*
 * grid_buttons.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "grid_buttons/grid_buttons.hpp"
#include "grid_buttons/GridControl.hpp"

#include "lcd/lcd.hpp" // for debugging

namespace grid
{

Grid::Grid() : gridControl( grid_control::GridControl::getInstance() )
{
}

void Grid::enable()
{
    if (initialized)
    {
        gridControl.startTimers();
    }
}

bool Grid::getButtonEvent(uint8_t* buttonPositionX, uint8_t* buttonPositionY, ButtonEvent* buttonEvent)
{
    int8_t x, y;
    uint8_t buttonColumnChanges, buttonInput;
    static bool buttonChangeDetected = false;

    if (gridControl.buttonInputUpdated || buttonChangeDetected)
    {
        buttonChangeDetected = false; //reset this variable every time, it will be set back if necessary
        gridControl.buttonInputUpdated = false;
        for (x=0; x<grid_control::NUMBER_OF_COLUMNS; x++)
        {
            if (gridControl.isGridColumnInputStable(x))
            {
                buttonInput = gridControl.getGridColumnInput(x);
                buttonColumnChanges = registeredGridButtonInput[x] ^ buttonInput;
                if (0 != buttonColumnChanges)
                {
                    for (y=0; y<grid_control::NUMBER_OF_ROWS; y++)
                    {
                        if (0 != ((buttonColumnChanges >> y) & 0x0001))
                        {
                            *buttonEvent = static_cast<ButtonEvent>((buttonInput >> y) & 0x01);
                            registeredGridButtonInput[x] ^= (1 << y); // toggle bit that was registered
                            if (x >= NUMBER_OF_COLUMNS)
                            {
                                x -= NUMBER_OF_COLUMNS;
                                y += grid_control::NUMBER_OF_ROWS;
                            }
                            *buttonPositionX = x;
                            *buttonPositionY = y;
                            buttonChangeDetected = true;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Grid::initialize()
{
    gridControl.initializeGpio();
    gridControl.initializeBaseInterruptTimer();
    gridControl.initializePwmOutputs();
    initialized = true;
}

void Grid::setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour)
{
    setLed(ledPositionX, ledPositionY, colour, LedLightingType_LIGHT);
}

void Grid::setLed(const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType)
{
    uint8_t i;
    // remove led from flashing or pulsing list if it's in that list and proceed with setting that led
    if (LedLightingType_FLASH == gridLed[ledPositionX][ledPositionY].lightingType)
    {
        for (i=0; i<numberOfFlashingLeds; i++)
        {
            if ((ledPositionX == flashingLed[i].positionX)&&(ledPositionY == flashingLed[i].positionY))
            {
                flashingLed[i] = flashingLed[numberOfFlashingLeds - 1]; // move last element into the place of element that is being removed
                numberOfFlashingLeds--;
                break;
            }
        }
    }
    else if (LedLightingType_PULSE == gridLed[ledPositionX][ledPositionY].lightingType)
    {
        for (i=0; i<numberOfPulsingLeds; i++)
        {
            if ((ledPositionX == pulsingLed[i].positionX)&&(ledPositionY == pulsingLed[i].positionY))
            {
                pulsingLed[i] = pulsingLed[numberOfPulsingLeds - 1]; // move last element into the place of element that is being removed
                numberOfPulsingLeds--;
                break;
            }
        }
    }

    switch (lightingType)
    {
        case LedLightingType_LIGHT:
            gridLed[ledPositionX][ledPositionY].colour = colour;
            gridLed[ledPositionX][ledPositionY].lightingType = LedLightingType_LIGHT;
            setLedColour(ledPositionX, ledPositionY, colour);
            break;
        case LedLightingType_FLASH:
            //save current color to have an alternate
            flashingLed[numberOfFlashingLeds].positionX = ledPositionX;
            flashingLed[numberOfFlashingLeds].positionY = ledPositionY;
            flashingLed[numberOfFlashingLeds].alternateColour = gridLed[ledPositionX][ledPositionY].colour;
            ++numberOfFlashingLeds;
            gridLed[ledPositionX][ledPositionY].lightingType = LedLightingType_FLASH;
            gridLed[ledPositionX][ledPositionY].colour = colour;
            setLedColour(ledPositionX, ledPositionY, colour);
            break;
        case LedLightingType_PULSE:
            //save current color to have an alternate
            pulsingLed[numberOfPulsingLeds].positionX = ledPositionX;
            pulsingLed[numberOfPulsingLeds].positionY = ledPositionY;
            ++numberOfPulsingLeds;
            gridLed[ledPositionX][ledPositionY].lightingType = LedLightingType_PULSE;
            gridLed[ledPositionX][ledPositionY].colour = colour;
            // don't change output value, it will be set on next pulse period
            break;
        default:
            break;
    }
}

void Grid::setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour )
{
    // evaluate if led is mounted under pad (more intensity), or to illuminate directly (less intensity)
    bool directLed = (ledPositionX > 7);

    if (ledPositionY >= grid_control::NUMBER_OF_ROWS)
    {
        ledPositionX += NUMBER_OF_COLUMNS;
        ledPositionY = ledPositionY % grid_control::NUMBER_OF_ROWS;
    }

    gridControl.setLedColour( ledPositionX, ledPositionY, directLed, colour );

}

Colour Grid::getLedColour(uint8_t ledPositionX, uint8_t ledPositionY)
{
    return gridLed[ledPositionX][ledPositionY].colour;
}

bool Grid::areColoursEqual(const Colour& colour1, const Colour& colour2)
{
    bool equal = true;
    equal &= (colour1.Red == colour2.Red);
    equal &= (colour1.Green == colour2.Green);
    equal &= (colour1.Blue == colour2.Blue);
    return equal;
}

void Grid::turnAllLedsOff()
{
    gridControl.turnAllLedsOff();

    // todo: also remove flashing, pulsing leds and reset colours to zeros
}


void Grid::refreshLeds()
{
    uint8_t i;
    static uint32_t ledFlashCheckTime = 0;
    static uint32_t ledPulseCheckTime = 0;
    static uint8_t ledPulseStepNumber = 0;
    static bool flashColourLocation = false;
    //consider changing this into else if
    if (HAL_GetTick() >= ledFlashCheckTime)
    {
        flashColourLocation = !flashColourLocation; // invert last byte
        for (i=0; i<numberOfFlashingLeds; i++)
        {
            if (flashColourLocation)
            {
                setLedColour(
                        flashingLed[i].positionX,
                        flashingLed[i].positionY,
                        gridLed[flashingLed[i].positionX][flashingLed[i].positionY].colour );
            }
            else
            {
                setLedColour(
                        flashingLed[i].positionX,
                        flashingLed[i].positionY,
                        flashingLed[i].alternateColour );
            }
        }
        ledFlashCheckTime = HAL_GetTick() + LED_FLASH_PERIOD_MS; //250ms
    }

    if (HAL_GetTick() >= ledPulseCheckTime)
    {
        struct Colour dimmedColour;
        ++ledPulseStepNumber;
        if (LED_PULSE_STEP_COUNT <= ledPulseStepNumber)
        {
            ledPulseStepNumber = 0;
        }

        for (i=0; i<numberOfPulsingLeds; i++)
        {
            dimmedColour = gridLed[pulsingLed[i].positionX][pulsingLed[i].positionY].colour;
            if (3 >= ledPulseStepNumber)
            {
                dimmedColour.Red = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
                dimmedColour.Green = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
                dimmedColour.Blue = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
            }
            else
            {
                dimmedColour.Red = (dimmedColour.Red * (19 - ledPulseStepNumber)) / 16;
                dimmedColour.Green = (dimmedColour.Green * (19 - ledPulseStepNumber)) / 16;
                dimmedColour.Blue = (dimmedColour.Blue * (19 - ledPulseStepNumber)) / 16;
            }
            setLedColour(pulsingLed[i].positionX, pulsingLed[i].positionY, dimmedColour);
        }
        ledPulseCheckTime = HAL_GetTick() + LED_PULSE_STEP_PERIOD_MS;
    }
}

}// namespace

