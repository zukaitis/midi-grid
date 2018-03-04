/*
 * grid_buttons_init.h
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#ifndef GRID_BUTTONS_GRID_BUTTONS_INIT_H_
#define GRID_BUTTONS_GRID_BUTTONS_INIT_H_

#include "grid_buttons/grid_buttons.hpp"
#include "stm32f4xx_hal.h"

#define GRID_BUTTON_IN_GPIO_PORT GPIOC
#define BUTTON_IN1_Pin GPIO_PIN_13
#define BUTTON_IN2_Pin GPIO_PIN_10
#define ROTARY1_A_Pin GPIO_PIN_14
#define ROTARY1_B_Pin GPIO_PIN_15
#define ROTARY2_A_Pin GPIO_PIN_11
#define ROTARY2_B_Pin GPIO_PIN_12
#define GRID_BUTTON_IN1_Pin GPIO_PIN_0
#define GRID_BUTTON_IN2_Pin GPIO_PIN_1
#define GRID_BUTTON_IN3_Pin GPIO_PIN_2
#define GRID_BUTTON_IN4_Pin GPIO_PIN_3

#define GRID_COLUMN_CONTROL_GPIO_PORT GPIOA
#define GRID_COLUMN_OUT1_Pin GPIO_PIN_8
#define GRID_COLUMN_OUT2_Pin GPIO_PIN_9
#define GRID_COLUMN_OUT3_Pin GPIO_PIN_10
#define GRID_COLUMN_OUT4_Pin GPIO_PIN_5
#define GRID_COLUMN_OUT5_Pin GPIO_PIN_4
#define GRID_COLUMN_OUT6_Pin GPIO_PIN_15

#define PWM_RED_GPIO_PORT GPIOA
#define PWM_RED1_Pin GPIO_PIN_0
#define PWM_RED2_Pin GPIO_PIN_1
#define PWM_RED3_Pin GPIO_PIN_2
#define PWM_RED4_Pin GPIO_PIN_3

#define PWM_GREEN_GPIO_PORT GPIOB
#define PWM_GREEN1_Pin GPIO_PIN_6
#define PWM_GREEN2_Pin GPIO_PIN_7
#define PWM_GREEN3_Pin GPIO_PIN_8
#define PWM_GREEN4_Pin GPIO_PIN_9

#define PWM_BLUE1_2_GPIO_PORT GPIOA
#define PWM_BLUE1_Pin GPIO_PIN_6
#define PWM_BLUE2_Pin GPIO_PIN_7
#define PWM_BLUE3_4_GPIO_PORT GPIOB
#define PWM_BLUE3_Pin GPIO_PIN_0
#define PWM_BLUE4_Pin GPIO_PIN_1

#define PWM_TIMER_RED           TIM2
#define PWM_TIMER_GREEN         TIM4
#define PWM_TIMER_BLUE          TIM3
#define BASE_INTERRUPT_TIMER    TIM10

#define NUMBER_OF_ROWS      4
#define NUMBER_OF_COLUMNS   20
#define NUMBER_OF_BUTTON_DEBOUNCING_CYCLES 2

#define LED_FLASH_PERIOD_MS         250 // 120bpm - default flashing rate
#define LED_PULSE_STEP_COUNT        15
#define LED_PULSE_STEP_PERIOD_MS    67  // 1000ms / 15 = 66.6... ms

#define PWM_CLOCK_PERIOD 47000 // <500us - has to be shorter than base period

#define GRID_BUTTON_MASK 0x000F

static const uint16_t columnSelectValue[NUMBER_OF_COLUMNS] = {  0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
                                                                0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
                                                                0x78FF, 0x7AFF, 0xF8EF, 0xF9EF,
                                                                0xFAEF, 0xFBEF, 0xFCEF, 0xFDEF,
                                                                0xFEEF, 0xFFEF, 0x79FF, 0x7BFF };

static const uint32_t baseInterruptClockPrescaler = 96; // 1us
static const uint32_t baseInterruptClockPeriod = 500; // 500us

static const uint16_t brightnessPad[65] = {
        47000, 46662, 46365, 46057, 45727, 45373, 44999, 44607,
        44195, 43763, 43317, 42851, 42373, 41879, 41359, 40843,
        40287, 39741, 39177, 38600, 37993, 37397, 36771, 36144,
        35499, 34851, 34185, 33501, 32796, 32108, 31429, 30721,
        29973, 29231, 28494, 27733, 26952, 26156, 25383, 24564,
        23781, 22934, 22103, 21275, 20401, 19570, 18699, 17802,
        16959, 16053, 15165, 14210, 13317, 12399, 11433, 10421,
        9485, 8522, 7585, 6597, 5642, 4627, 3641, 2591, 1001 };

static const uint16_t brightnessDirect[65] = {
        47000, 46777, 46692, 46603, 46506, 46402, 46291, 46175,
        46053, 45925, 45792, 45652, 45509, 45361, 45207, 45050,
        44887, 44721, 44552, 44379, 44198, 44019, 43836, 43646,
        43458, 43262, 43069, 42867, 42663, 42458, 42252, 42048,
        41834, 41618, 41409, 41191, 40968, 40741, 40527, 40301,
        40078, 39845, 39615, 39393, 39144, 38921, 38681, 38449,
        38217, 37979, 37735, 37500, 37247, 37005, 36767, 36511,
        36263, 36011, 35787, 35511, 35271, 35013, 34797, 34520, 34259 };

struct LedPwmOutput
{
    uint16_t Red;
    uint16_t Green;
    uint16_t Blue;
};

struct FlashingLed
{
    uint8_t positionX;
    uint8_t positionY;
    struct Colour alternateColour;
};

struct PulsingLed
{
    uint8_t positionX;
    uint8_t positionY;
};

struct GridLed
{
    struct Colour colour;
    enum LedLightingType ledLightingType; // light?flash?pulse
};

void grid_initializeBaseInterruptTimer();
void grid_initializeGpio();
void grid_initializePwmOutputs();
void grid_startTimers();

#endif /* GRID_BUTTONS_GRID_BUTTONS_INIT_H_ */
