/*
 * grid_buttons_init.h
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#ifndef GRID_BUTTONS_GRID_BUTTONS_INIT_H_
#define GRID_BUTTONS_GRID_BUTTONS_INIT_H_

#include "grid_buttons/grid_buttons.h"
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
    uint8_t lightingType; // light?flash?pulse
};

static const struct Colour launchpadColourPalette[128] = {
        {0, 0, 0}, {8, 8, 8}, {32, 32, 32}, {64, 64, 64}, {64, 20, 18}, {64, 3, 0}, {23, 1, 0}, {7, 0, 0},
        {64, 48, 25}, {64, 22, 0}, {23, 8, 0}, {10, 7, 0}, {64, 64, 9}, {64, 64, 0}, {23, 23, 0}, {6, 6, 0},
        {33, 64, 11}, {17, 64, 0}, {6, 23, 0}, {5, 11, 0}, {14, 64, 11}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0},
        {13, 64, 18}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0}, {13, 64, 32}, {0, 64, 15}, {0, 23, 6}, {0, 8, 4},
        {12, 64, 45}, {0, 64, 37}, {0, 23, 13}, {0, 7, 4}, {15, 48, 64}, {0, 42, 64}, {0, 17, 21}, {0, 5, 7},
        {17, 34, 64}, {0, 21, 64}, {0, 7, 23}, {0, 2, 7}, {18, 18, 64}, {0, 0, 64}, {0, 0, 23}, {0, 0, 7},
        {33, 18, 64}, {21, 0, 64}, {6, 0, 26}, {3, 0, 13}, {64, 19, 64}, {64, 0, 64}, {23, 0, 23}, {7, 0, 7},
        {64, 20, 34}, {64, 2, 21}, {23, 1, 7}, {9, 0, 5}, {64, 7, 0}, {39, 14, 0}, {31, 21, 0}, {16, 26, 0},
        {0, 15, 0}, {0, 22, 13}, {0, 21, 32}, {0, 0, 64}, {0, 18, 20}, {7, 0, 53}, {32, 32, 32}, {9, 9, 9},
        {64, 3, 0}, {47, 64, 0}, {43, 60, 0}, {22, 64, 0}, {0, 35, 0}, {0, 64, 31}, {0, 42, 64}, {0, 7, 64},
        {14, 0, 64}, {30, 0, 64}, {46, 6, 32}, {17, 9, 0}, {64, 19, 0}, {33, 57, 0}, {26, 64, 0}, {0, 64, 0},
        {0, 64, 0}, {18, 64, 25}, {0, 64, 51}, {21, 34, 64}, {10, 20, 51}, {33, 31, 60}, {53, 4, 64}, {64, 2, 23},
        {64, 32, 0}, {47, 45, 0}, {35, 64, 0}, {33, 24, 0}, {14, 11, 0}, {4, 20, 2}, {0, 21, 14}, {5, 5, 11},
        {5, 8, 23}, {27, 16, 6}, {44, 2, 0}, {64, 21, 14}, {56, 27, 0}, {64, 64, 0}, {39, 57, 0}, {24, 46, 0},
        {7, 7, 13}, {56, 64, 21}, {30, 64, 47}, {38, 38, 64}, {35, 25, 64}, {17, 17, 17}, {30, 30, 30}, {56, 64, 64},
        {42, 2, 0}, {14, 0, 0}, {0, 53, 0}, {0, 17, 0}, {47, 45, 0}, {16, 13, 0}, {46, 24, 0}, {19, 6, 0} };

void grid_initializeBaseInterruptTimer();
void grid_initializeGpio();
void grid_initializePwmOutputs();
void grid_startTimers();

#endif /* GRID_BUTTONS_GRID_BUTTONS_INIT_H_ */
