/*
 * GridControl.cpp
 *
 *  Created on: 2018-03-06
 *      Author: Gedas
 */

#include "grid_buttons/GridControl.hpp"

namespace grid_control
{

GridControl::GridControl()
{
    uint8_t i, j;
    // set array values here
    for (i=0; i<NUMBER_OF_COLUMNS; i++)
    {
        for (j=0; j<NUMBER_OF_ROWS; j++)
        {
            ledOutput[i][j] = LED_PASSIVE;
        }
    }
}

GridControl::~GridControl()
{
}

void GridControl::setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, bool directLed, const grid::Colour colour )
{
    if (0 == ledPositionX)
    {
        ledPositionX = NUMBER_OF_COLUMNS - 1;
    }
    else
    {
        --ledPositionX;
    }

    if (directLed)
    {
        ledOutput[ledPositionX][ledPositionY].Red = brightnessDirect[colour.Red];
        ledOutput[ledPositionX][ledPositionY].Green = brightnessDirect[colour.Green];
        ledOutput[ledPositionX][ledPositionY].Blue = brightnessDirect[colour.Blue];
    }
    else
    {
        ledOutput[ledPositionX][ledPositionY].Red = brightnessPad[colour.Red];
        ledOutput[ledPositionX][ledPositionY].Green = brightnessPad[colour.Green];
        ledOutput[ledPositionX][ledPositionY].Blue = brightnessPad[colour.Blue];
    }
}

void GridControl::turnAllLedsOff()
{
    uint8_t x, y;
    for (x = 0; x < NUMBER_OF_COLUMNS; x++)
    {
        for (y = 0; y < NUMBER_OF_ROWS; y++)
        {
            ledOutput[x][y] = LED_PASSIVE;
        }
    }
}

bool GridControl::isGridColumnInputStable(const uint8_t column)
{
    return (0 == (GRID_BUTTON_MASK & (buttonInput[column][0] ^ buttonInput[column][1])));
}

uint8_t GridControl::getGridColumnInput(const uint8_t column)
{
    return static_cast<uint8_t>(GRID_BUTTON_MASK & buttonInput[column][0]);
}

void GridControl::initializeBaseInterruptTimer()
{
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_MasterConfigTypeDef timerMasterConfiguration;

    __HAL_RCC_TIM10_CLK_ENABLE();

    baseInterruptTimer.Instance = BASE_INTERRUPT_TIMER;
    baseInterruptTimer.Init.Prescaler = BASE_INTERRUPT_CLOCK_PRESCALER - 1; // 1us is the desired timer step
    baseInterruptTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    baseInterruptTimer.Init.Period = BASE_INTERRUPT_CLOCK_PERIOD - 1;  // 1s / (4*100) = 2.5ms
    baseInterruptTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&baseInterruptTimer);

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&baseInterruptTimer, &timerClockSourceConfiguration);

    timerMasterConfiguration.MasterOutputTrigger = TIM_TRGO_UPDATE;
    timerMasterConfiguration.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization( &baseInterruptTimer, &timerMasterConfiguration );

    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void GridControl::initializeGpio()
{
      GPIO_InitTypeDef gpioConfiguration;

      /* GPIO Ports Clock Enable */
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      /*Configure GPIO pin Output Level */
      HAL_GPIO_WritePin( GRID_COLUMN_CONTROL_GPIO_PORT, GRID_COLUMN_OUT1_Pin|GRID_COLUMN_OUT2_Pin|GRID_COLUMN_OUT3_Pin|GRID_COLUMN_OUT4_Pin
                              |GRID_COLUMN_OUT5_Pin|GRID_COLUMN_OUT6_Pin, GPIO_PIN_SET );

      /*Configure GPIO pins : BUTTON_IN1_Pin ROTARY1_A_Pin ROTARY1_B_Pin BUTTON_IN2_Pin
                               ROTARY2_A_Pin ROTARY2_B_Pin */
      gpioConfiguration.Pin = BUTTON_IN1_Pin|BUTTON_IN2_Pin|ROTARY1_A_Pin|ROTARY1_B_Pin
                              |ROTARY2_A_Pin|ROTARY2_B_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLUP;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      /*Configure GPIO pins : GRID_BUTTON_IN1_Pin GRID_BUTTON_IN2_Pin GRID_BUTTON_IN3_Pin GRID_BUTTON_IN4_Pin
                               MIDI_OUT_DETECT_Pin */
      gpioConfiguration.Pin = GRID_BUTTON_IN1_Pin|GRID_BUTTON_IN2_Pin|GRID_BUTTON_IN3_Pin|GRID_BUTTON_IN4_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLDOWN;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      /*Configure GPIO pins : GRID_COLUMN_OUT5_Pin GRID_COLUMN_OUT4_Pin GRID_COLUMN_OUT1_Pin GRID_COLUMN_OUT2_Pin
                               GRID_COLUMN_OUT3_Pin GRID_COLUMN_OUT6_Pin */
      gpioConfiguration.Pin = GRID_COLUMN_OUT1_Pin|GRID_COLUMN_OUT2_Pin|GRID_COLUMN_OUT3_Pin|GRID_COLUMN_OUT4_Pin
                              |GRID_COLUMN_OUT5_Pin|GRID_COLUMN_OUT6_Pin;
      gpioConfiguration.Mode = GPIO_MODE_OUTPUT_OD;
      gpioConfiguration.Pull = GPIO_NOPULL;
      gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      HAL_GPIO_Init( GRID_COLUMN_CONTROL_GPIO_PORT, &gpioConfiguration );
}

void GridControl::initializePwmOutputs()
{
    GPIO_InitTypeDef gpioConfiguration;
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_SlaveConfigTypeDef timerSlaveConfiguration;
    TIM_OC_InitTypeDef timerOutputCompareConfiguration;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Same configuration for all three channels
    timerSlaveConfiguration.SlaveMode = TIM_SLAVEMODE_RESET;

    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_PWM2;
    timerOutputCompareConfiguration.Pulse = PWM_CLOCK_PERIOD; // start with passive output
    timerOutputCompareConfiguration.OCPolarity = TIM_OCPOLARITY_HIGH;
    timerOutputCompareConfiguration.OCFastMode = TIM_OCFAST_DISABLE;

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    // Red PWM output configuration
    pwmTimerRed.Instance = PWM_TIMER_RED;
    pwmTimerRed.Init.Prescaler = 0;
    pwmTimerRed.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerRed.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerRed.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerRed );

    HAL_TIM_ConfigClockSource( &pwmTimerRed, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerRed );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerRed, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    // Green PWM output configuration
    pwmTimerGreen.Instance = PWM_TIMER_GREEN;
    pwmTimerGreen.Init.Prescaler = 0;
    pwmTimerGreen.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerGreen.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerGreen.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerGreen );

    HAL_TIM_ConfigClockSource( &pwmTimerGreen, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerGreen );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerGreen, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    // Blue PWM output configuration
    pwmTimerBlue.Instance = PWM_TIMER_BLUE;
    pwmTimerBlue.Init.Prescaler = 0;
    pwmTimerBlue.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerBlue.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerBlue.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerBlue );

    HAL_TIM_ConfigClockSource( &pwmTimerBlue, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerBlue );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerBlue, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    // Timer GPIO configuration
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Pull = GPIO_PULLDOWN;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    gpioConfiguration.Pin = PWM_RED1_Pin|PWM_RED2_Pin|PWM_RED3_Pin|PWM_RED4_Pin;
    gpioConfiguration.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init( PWM_RED_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE1_Pin|PWM_BLUE2_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE1_2_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE3_Pin|PWM_BLUE4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE3_4_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_GREEN1_Pin|PWM_GREEN2_Pin|PWM_GREEN3_Pin|PWM_GREEN4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init( PWM_GREEN_GPIO_PORT, &gpioConfiguration );
}

void GridControl::startTimers()
{
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerRed);

    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerGreen);

    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerBlue);

    HAL_TIM_Base_Start_IT(&baseInterruptTimer);
}

// Grid base interrupt, speed is the factor here, so all operations are performed directly with registers
void GridControl::interruptServiceRoutine()
{
    static uint8_t currentColumnNumber = 0;
    static uint8_t currentColumnDebouncingIndex = 0;
    // Clear interrupt flag
    BASE_INTERRUPT_TIMER->SR = ~TIM_FLAG_UPDATE;

    buttonInput[currentColumnNumber][currentColumnDebouncingIndex] = GRID_BUTTON_IN_GPIO_PORT->IDR;

    ++currentColumnNumber;
    if (NUMBER_OF_COLUMNS == currentColumnNumber)
    {
        currentColumnDebouncingIndex ^= 0x01; // switch debouncing index between 0 and 1
        currentColumnNumber = 0;
        buttonInputUpdated = true;
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

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
    static GridControl& gridControl_ = GridControl::getInstance();
    gridControl_.interruptServiceRoutine();
}

} // namespace
