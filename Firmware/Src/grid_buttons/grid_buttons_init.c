/*
 * grid_buttons_init.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "main.h" // TODO: move definitions out of main.h
#include "stm32f4xx_hal.h"

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
}

grid_initializePwmOutputs()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

#if 0
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 48999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

    HAL_TIM_PWM_Init(&htim2);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

    sConfigOC.OCMode = TIM_OCMODE_PWM2;
    sConfigOC.Pulse = 48999;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);
#endif

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Pin = PWM_RED1_Pin|PWM_RED2_Pin|PWM_RED3_Pin|PWM_RED4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWM_BLUE1_Pin|PWM_BLUE2_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWM_BLUE3_Pin|PWM_BLUE4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWM_GREEN1_Pin|PWM_GREEN2_Pin|PWM_GREEN3_Pin|PWM_GREEN4_Pin;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
