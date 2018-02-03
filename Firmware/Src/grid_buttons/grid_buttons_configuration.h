/*
 * grid_buttons_init.h
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#ifndef GRID_BUTTONS_GRID_BUTTONS_INIT_H_
#define GRID_BUTTONS_GRID_BUTTONS_INIT_H_

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

#define PWM_CLOCK_PERIOD 48000 // 500us

static const uint16_t columnSelectValue[NUMBER_OF_COLUMNS] = {  0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
                                                                0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
                                                                0xF8EF, 0xF9EF, 0xFAEF, 0xFBEF,
                                                                0xFCEF, 0xFDEF, 0xFEEF, 0xFFEF,
                                                                0x78FF, 0x79FF, 0x7AFF, 0x7BFF };

static const uint32_t baseInterruptClockPrescaler = 96; // 1us
static const uint32_t baseInterruptClockPeriod = 500; // 500us

static const uint16_t brightnessRed[65] = { 48000, 47995, 47977, 47943, 47893, 47824, 47738, 47632,
                                            47506, 47360, 47193, 47005, 46795, 46563, 46309, 46031,
                                            45731, 45407, 45060, 44689, 44293, 43873, 43428, 42959,
                                            42464, 41944, 41398, 40826, 40229, 39605, 38955, 38278,
                                            37575, 36845, 36088, 35303, 34492, 33652, 32785, 31891,
                                            30968, 30017, 29038, 28030, 26994, 25930, 24836, 23714,
                                            22563, 21382, 20173, 18933, 17665, 16367, 15039, 13681,
                                            12293, 10875, 9427, 7949, 6440, 4901, 3332, 1731, 100 };

static uint8_t gridInitializationDone = 0;

struct LedPwmOutput
{
    uint16_t Red;
    uint16_t Green;
    uint16_t Blue;
};

void grid_initializeBaseInterruptTimer();
void grid_initializeGpio();
void grid_initializePwmOutputs();
void grid_startTimers();

#endif /* GRID_BUTTONS_GRID_BUTTONS_INIT_H_ */
