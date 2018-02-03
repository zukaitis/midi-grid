/*
 * grid_buttons.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "grid_buttons/grid_buttons_configuration.h"
#include "stm32f4xx_hal.h"
#define LED_PASSIVE {.Red = PWM_CLOCK_PERIOD, .Green = PWM_CLOCK_PERIOD, .Blue = PWM_CLOCK_PERIOD}
//#define LED_PASSIVE {.Red = 100, .Green = 100, .Blue = 100}

static uint8_t currentColumnNumber = 0;
static uint8_t currentColumnDebouncingIndex = 0;
static uint8_t buttonInputUpdatedFlag = 1;
static uint16_t buttonInput[NUMBER_OF_COLUMNS][NUMBER_OF_BUTTON_DEBOUNCING_CYCLES];

static struct LedPwmOutput ledOutput[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS] = {
    {{2000, PWM_CLOCK_PERIOD, PWM_CLOCK_PERIOD}, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {{PWM_CLOCK_PERIOD, 1000, PWM_CLOCK_PERIOD}, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {{PWM_CLOCK_PERIOD, PWM_CLOCK_PERIOD, 100}, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE},
    {LED_PASSIVE, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE}, {{2000, 1000, 100}, LED_PASSIVE, LED_PASSIVE, LED_PASSIVE} };

void grid_enable()
{
    if( gridInitializationDone )
    {
        grid_startTimers();
    }
}

void grid_initialize()
{
    grid_initializeGpio();
    grid_initializeBaseInterruptTimer();
    grid_initializePwmOutputs();
    gridInitializationDone = 1;
}

void grid_setLedColourFromLaunchpadPalette(uint8_t ledPositionX, uint8_t ledPositionY, uint8_t colourNumber)
{

}

void grid_setLedOutputValues(uint8_t ledPositionX, uint8_t ledPositionY, uint16_t outputRed, uint16_t outputGreen, uint16_t outputBlue)
{

}

void grid_setOutput()
{
    GPIOA->ODR = 0xF8DF;
    GPIOB->ODR = PWM_GREEN1_Pin;
}



// Grid base interrupt, speed is the factor here, so all operations are performed directly with registers
void TIM1_UP_TIM10_IRQHandler()
{
    //__HAL_TIM_CLEAR_FLAG (&gridBaseTimer, TIM_FLAG_UPDATE);
    BASE_INTERRUPT_TIMER->SR = ~TIM_FLAG_UPDATE;

    buttonInput[currentColumnNumber][currentColumnDebouncingIndex] = GPIOD->IDR;

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

    // TODO: configure master to reset these timers automatically
//    PWM_TIMER_RED->CNT = 0;
//    PWM_TIMER_GREEN->CNT = 0;
//    PWM_TIMER_BLUE->CNT = 0;
}

