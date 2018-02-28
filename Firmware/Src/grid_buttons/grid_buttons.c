/*
 * grid_buttons.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "grid_buttons/grid_buttons.h"
#include "grid_buttons/grid_buttons_configuration.h"

#include "lcd/lcd.h" // for debugging

#define LED_PASSIVE {.Red = PWM_CLOCK_PERIOD, .Green = PWM_CLOCK_PERIOD, .Blue = PWM_CLOCK_PERIOD}
//#define LED_PASSIVE {.Red = 40000, .Green = 8200, .Blue = 40000}

static uint8_t currentColumnNumber = 0;
static uint8_t currentColumnDebouncingIndex = 0;
static uint8_t buttonInputUpdatedFlag = 1;
static uint16_t buttonInput[NUMBER_OF_COLUMNS][NUMBER_OF_BUTTON_DEBOUNCING_CYCLES];
static uint8_t gridInitializationDone = 0;

static struct GridLed gridLed[10][8];

static struct LedPwmOutput ledOutput[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS] = {
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
        {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE} };

static const struct LedPwmOutput ledPassive = LED_PASSIVE;

static uint16_t registeredGridButtonInput[NUMBER_OF_COLUMNS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static struct FlashingLed flashingLed[64];
static uint8_t numberOfFlashingLeds = 0;

static struct PulsingLed pulsingLed[64];
static uint8_t numberOfPulsingLeds = 0;

void grid_enable()
{
    if (gridInitializationDone)
    {
        grid_startTimers();
    }
}

uint8_t grid_getButtonEvent(uint8_t* buttonPositionX, uint8_t* buttonPositionY, uint8_t* buttonEvent)
{
    int8_t x, y;
    uint16_t isInputValueStable;
    uint16_t buttonColumnChanges;
    static uint8_t buttonChangeDetected = 0;
    if (buttonInputUpdatedFlag || buttonChangeDetected)
    {
        buttonChangeDetected = 0; //reset this variable every time, it will be set back if necessary
        buttonInputUpdatedFlag = 0;
        for (x=0; x<NUMBER_OF_COLUMNS; x++)
        {
            buttonColumnChanges = registeredGridButtonInput[x] ^ (GRID_BUTTON_MASK & buttonInput[x][0]);
            if (0 != buttonColumnChanges)
            {
                // XOR both acquired input values of one column, to see whether they are equal
                isInputValueStable = GRID_BUTTON_MASK & (buttonInput[x][0] ^ buttonInput[x][1]);
                if (0 == isInputValueStable)
                {
                    for (y=0; y<NUMBER_OF_ROWS; y++)
                    {
                        if (0 != ((buttonColumnChanges >> y) & 0x0001))
                        {
                            *buttonEvent = (buttonInput[x][0] >> y) & 0x01;
                            registeredGridButtonInput[x] ^= (1 << y); // toggle bit that was registered
                            if (x > 9)
                            {
                                x -= 10;
                                y += 4;
                            }
                            *buttonPositionX = x;
                            *buttonPositionY = y;
                            buttonChangeDetected = 1;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void grid_initialize()
{
    grid_initializeGpio();
    grid_initializeBaseInterruptTimer();
    grid_initializePwmOutputs();
    gridInitializationDone = 1;
}

void grid_setLed(uint8_t ledPositionX, uint8_t ledPositionY, const struct Colour* colour, enum LedLightingType ledLightingType)
{
    uint8_t i;
    // remove led from flashing or pulsing list if it's in that list and proceed with setting that led
    if (LedLightingType_FLASH == gridLed[ledPositionX][ledPositionY].ledLightingType)
    {
        for (i=0; i<numberOfFlashingLeds; i++)
        {
            if ((ledPositionX == flashingLed[i].positionX)&&(ledPositionY == flashingLed[i].positionY))
            {
                flashingLed[i] = flashingLed[numberOfFlashingLeds - 1]; // move last element into the place of element that is being removed
                //i = numberOfFlashingLeds; // break loop
                numberOfFlashingLeds--;
                break;
            }
        }
    }
    else if (LedLightingType_PULSE == gridLed[ledPositionX][ledPositionY].ledLightingType)
    {
        for (i=0; i<numberOfPulsingLeds; i++)
        {
            if ((ledPositionX == pulsingLed[i].positionX)&&(ledPositionY == pulsingLed[i].positionY))
            {
                pulsingLed[i] = pulsingLed[numberOfPulsingLeds - 1]; // move last element into the place of element that is being removed
                //i = numberOfPulsingLeds; // break loop
                numberOfPulsingLeds--;
                break;
            }
        }
    }

    switch (ledLightingType)
    {
        case LedLightingType_LIGHT:
            gridLed[ledPositionX][ledPositionY].colour = *colour;
            gridLed[ledPositionX][ledPositionY].ledLightingType = LedLightingType_LIGHT;
            grid_setLedColour(ledPositionX, ledPositionY, colour);
            break;
        case LedLightingType_FLASH:
            //save current color to have an alternate
            flashingLed[numberOfFlashingLeds].positionX = ledPositionX;
            flashingLed[numberOfFlashingLeds].positionY = ledPositionY;
            flashingLed[numberOfFlashingLeds].alternateColour = gridLed[ledPositionX][ledPositionY].colour;
            ++numberOfFlashingLeds;
            gridLed[ledPositionX][ledPositionY].ledLightingType = LedLightingType_FLASH;
            gridLed[ledPositionX][ledPositionY].colour = *colour;
            grid_setLedColour(ledPositionX, ledPositionY, colour);
            break;
        case LedLightingType_PULSE:
            //save current color to have an alternate
            pulsingLed[numberOfPulsingLeds].positionX = ledPositionX;
            pulsingLed[numberOfPulsingLeds].positionY = ledPositionY;
            ++numberOfPulsingLeds;
            gridLed[ledPositionX][ledPositionY].ledLightingType = LedLightingType_PULSE;
            gridLed[ledPositionX][ledPositionY].colour = *colour;
            // don't change output value, it will be set on next pulse period
            break;
        default:
            break;
    }
}

void grid_setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const struct Colour* colour )
{
    // evaluate if led is mounted under pad (more intensity), or to illuminate directly (less intensity)
    uint8_t directLed = (ledPositionX > 7) ? 1 : 0;


    if (ledPositionY > 3)
    {
        ledPositionX += 9; // + 10 - 1
        ledPositionY = ledPositionY % 4;
    }
    else if (0 == ledPositionX)
    {
        ledPositionX = NUMBER_OF_COLUMNS - 1;
    }
    else
    {
        --ledPositionX;
    }

    if (directLed)
    {
        ledOutput[ledPositionX][ledPositionY].Red = brightnessDirect[colour->Red];
        ledOutput[ledPositionX][ledPositionY].Green = brightnessDirect[colour->Green];
        ledOutput[ledPositionX][ledPositionY].Blue = brightnessDirect[colour->Blue];
    }
    else
    {
        ledOutput[ledPositionX][ledPositionY].Red = brightnessPad[colour->Red];
        ledOutput[ledPositionX][ledPositionY].Green = brightnessPad[colour->Green];
        ledOutput[ledPositionX][ledPositionY].Blue = brightnessPad[colour->Blue];
    }
}

struct Colour grid_getLedColour(uint8_t ledPositionX, uint8_t ledPositionY)
{
    return gridLed[ledPositionX][ledPositionY].colour;
}

uint8_t grid_areColoursEqual(const struct Colour * colour1, const struct Colour * colour2)
{
    uint8_t equal = 0x01;
    equal &= (colour1->Red == colour2->Red);
    equal &= (colour1->Green == colour2->Green);
    equal &= (colour1->Blue == colour2->Blue);
    return equal;
}

void grid_setAllLedsOff()
{
    uint8_t x, y;
    for (x = 0; x < NUMBER_OF_COLUMNS; x++)
    {
        for (y = 0; y < NUMBER_OF_ROWS; y++)
        {
            ledOutput[x][y] = ledPassive;
        }
    }
}


void grid_setLedOutputDirectly( uint8_t ledPositionX, uint8_t ledPositionY, uint16_t outputRed, uint16_t outputGreen, uint16_t outputBlue )
{
    if (ledPositionY > 3)
    {
        ledPositionX += 9; // + 10 - 1
        ledPositionY = ledPositionY % 4;
    }
    else if (0 == ledPositionX)
    {
        ledPositionX = NUMBER_OF_COLUMNS - 1;
    }
    else
    {
        --ledPositionX;
    }

    ledOutput[ledPositionX][ledPositionY].Red = outputRed;
    ledOutput[ledPositionX][ledPositionY].Green = outputGreen;
    ledOutput[ledPositionX][ledPositionY].Blue = outputBlue;
}

void grid_updateLeds()
{
    uint8_t i;
    static uint32_t ledFlashCheckTime = 0;
    static uint32_t ledPulseCheckTime = 0;
    static uint8_t ledPulseStepNumber = 0;
    static uint8_t nextFlashColour = 0; // requires renaming
    //consider changing this into else if
    if (HAL_GetTick() >= ledFlashCheckTime)
    {
        nextFlashColour ^= 0x01; // invert last byte
        for (i=0; i<numberOfFlashingLeds; i++)
        {
            if (0 == nextFlashColour)
            {
                grid_setLedColour(
                        flashingLed[i].positionX,
                        flashingLed[i].positionY,
                        &gridLed[flashingLed[i].positionX][flashingLed[i].positionY].colour );
            }
            else
            {
                grid_setLedColour(
                        flashingLed[i].positionX,
                        flashingLed[i].positionY,
                        &flashingLed[i].alternateColour );
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
            grid_setLedColour(pulsingLed[i].positionX, pulsingLed[i].positionY, &dimmedColour);
        }
        ledPulseCheckTime = HAL_GetTick() + LED_PULSE_STEP_PERIOD_MS;
    }
}

// Grid base interrupt, speed is the factor here, so all operations are performed directly with registers
void TIM1_UP_TIM10_IRQHandler()
{
    // Clear interrupt flag
    BASE_INTERRUPT_TIMER->SR = ~TIM_FLAG_UPDATE;

    buttonInput[currentColumnNumber][currentColumnDebouncingIndex] = GRID_BUTTON_IN_GPIO_PORT->IDR;

    ++currentColumnNumber;
    if (NUMBER_OF_COLUMNS == currentColumnNumber)
    {
        currentColumnDebouncingIndex ^= 0x01; // switch debouncing index between 0 and 1
        currentColumnNumber = 0;
        buttonInputUpdatedFlag = 1;
    }

    GRID_COLUMN_CONTROL_GPIO_PORT->ODR = columnSelectValue[currentColumnNumber];

    // CCR values set at interrupt are only compared after update (at the next period)
    PWM_TIMER_RED->CCR1 = ledOutput[currentColumnNumber][0].Red;
    PWM_TIMER_RED->CCR2 = ledOutput[currentColumnNumber][1].Red;
    PWM_TIMER_RED->CCR3 = ledOutput[currentColumnNumber][2].Red;
    PWM_TIMER_RED->CCR4 = ledOutput[currentColumnNumber][3].Red;

    PWM_TIMER_GREEN->CCR1 = ledOutput[currentColumnNumber][0].Green;
    PWM_TIMER_GREEN->CCR2 = ledOutput[currentColumnNumber][1].Green;
    PWM_TIMER_GREEN->CCR3 = ledOutput[currentColumnNumber][2].Green;
    PWM_TIMER_GREEN->CCR4 = ledOutput[currentColumnNumber][3].Green;

    PWM_TIMER_BLUE->CCR1 = ledOutput[currentColumnNumber][0].Blue;
    PWM_TIMER_BLUE->CCR2 = ledOutput[currentColumnNumber][1].Blue;
    PWM_TIMER_BLUE->CCR3 = ledOutput[currentColumnNumber][2].Blue;
    PWM_TIMER_BLUE->CCR4 = ledOutput[currentColumnNumber][3].Blue;

    // this should be done automatically through slave reset
    PWM_TIMER_RED->CNT = 0;
    PWM_TIMER_GREEN->CNT = 0;
    PWM_TIMER_BLUE->CNT = 0;
}

