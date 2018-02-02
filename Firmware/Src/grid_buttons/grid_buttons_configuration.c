/*
 * grid_buttons_init.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "grid_buttons/grid_buttons_configuration.h"
#include "stm32f4xx_hal.h"

#if 0
static TIM_HandleTypeDef pwmTimerRed;
static TIM_HandleTypeDef pwmTimerGreen;
static TIM_HandleTypeDef pwmTimerBlue;
#endif

void grid_initializeGpio()
{
	  GPIO_InitTypeDef GPIO_InitStruct;

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(GPIOA, GRID_COLUMN_OUT1_Pin|GRID_COLUMN_OUT2_Pin|GRID_COLUMN_OUT3_Pin|GRID_COLUMN_OUT4_Pin
	                          |GRID_COLUMN_OUT5_Pin|GRID_COLUMN_OUT6_Pin, GPIO_PIN_SET);

	  /*Configure GPIO pins : BUTTON_IN1_Pin ROTARY1_A_Pin ROTARY1_B_Pin BUTTON_IN2_Pin
	                           ROTARY2_A_Pin ROTARY2_B_Pin */
	  GPIO_InitStruct.Pin = BUTTON_IN1_Pin|BUTTON_IN2_Pin|ROTARY1_A_Pin|ROTARY1_B_Pin
	                          |ROTARY2_A_Pin|ROTARY2_B_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  /*Configure GPIO pins : GRID_BUTTON_IN1_Pin GRID_BUTTON_IN2_Pin GRID_BUTTON_IN3_Pin GRID_BUTTON_IN4_Pin
	                           MIDI_OUT_DETECT_Pin */
	  GPIO_InitStruct.Pin = GRID_BUTTON_IN1_Pin|GRID_BUTTON_IN2_Pin|GRID_BUTTON_IN3_Pin|GRID_BUTTON_IN4_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  /*Configure GPIO pins : GRID_COLUMN_OUT5_Pin GRID_COLUMN_OUT4_Pin GRID_COLUMN_OUT1_Pin GRID_COLUMN_OUT2_Pin
	                           GRID_COLUMN_OUT3_Pin GRID_COLUMN_OUT6_Pin */
	  GPIO_InitStruct.Pin = GRID_COLUMN_OUT1_Pin|GRID_COLUMN_OUT2_Pin|GRID_COLUMN_OUT3_Pin|GRID_COLUMN_OUT4_Pin
	                          |GRID_COLUMN_OUT5_Pin|GRID_COLUMN_OUT6_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pin = PWM_BLUE1_Pin|PWM_RED1_Pin;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = PWM_GREEN1_Pin;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
#if 0
grid_initializePwmOutputs()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_SlaveConfigTypeDef timerSlaveConfiguration;
    TIM_OC_InitTypeDef timerOutputCompareConfiguration;

    // same configuration for all three channels
    timerSlaveConfiguration.SlaveMode = TIM_SLAVEMODE_RESET;

    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_PWM2;
    timerOutputCompareConfiguration.Pulse = pwmClockPeriod - 1; // start with passive output
    timerOutputCompareConfiguration.OCPolarity = TIM_OCPOLARITY_HIGH;
    timerOutputCompareConfiguration.OCFastMode = TIM_OCFAST_DISABLE;

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    // Red PWM output configuration
    pwmTimerRed.Instance = TIM2;
    pwmTimerRed.Init.Prescaler = 0;
    pwmTimerRed.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerRed.Init.Period = pwmClockPeriod - 1;
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
    pwmTimerGreen.Instance = TIM4;
    pwmTimerGreen.Init.Prescaler = 0;
    pwmTimerGreen.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerGreen.Init.Period = pwmClockPeriod - 1;
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
    pwmTimerBlue.Instance = TIM4;
    pwmTimerBlue.Init.Prescaler = 0;
    pwmTimerBlue.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerBlue.Init.Period = pwmClockPeriod - 1;
    pwmTimerBlue.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerBlue );

    HAL_TIM_ConfigClockSource( &pwmTimerBlue, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerBlue);

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerBlue, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    // Timer GPIO configuration
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Pin = PWM_RED1_Pin|PWM_RED2_Pin|PWM_RED3_Pin|PWM_RED4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = PWM_BLUE1_Pin|PWM_BLUE2_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = PWM_BLUE3_Pin|PWM_BLUE4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );

    GPIO_InitStruct.Pin = PWM_GREEN1_Pin|PWM_GREEN2_Pin|PWM_GREEN3_Pin|PWM_GREEN4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );
}
#endif
