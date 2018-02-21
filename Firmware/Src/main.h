/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
//#define USE_SEMIHOSTING
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define BUTTON_IN1_Pin GPIO_PIN_13
#define BUTTON_IN1_GPIO_Port GPIOC
#define ROTARY1_A_Pin GPIO_PIN_14
#define ROTARY1_A_GPIO_Port GPIOC
#define ROTARY1_B_Pin GPIO_PIN_15
#define ROTARY1_B_GPIO_Port GPIOC
#define GRID_BUTTON_IN1_Pin GPIO_PIN_0
#define GRID_BUTTON_IN1_GPIO_Port GPIOC
#define GRID_BUTTON_IN2_Pin GPIO_PIN_1
#define GRID_BUTTON_IN2_GPIO_Port GPIOC
#define GRID_BUTTON_IN3_Pin GPIO_PIN_2
#define GRID_BUTTON_IN3_GPIO_Port GPIOC
#define GRID_BUTTON_IN4_Pin GPIO_PIN_3
#define GRID_BUTTON_IN4_GPIO_Port GPIOC
#define PWM_RED1_Pin GPIO_PIN_0
#define PWM_RED1_GPIO_Port GPIOA
#define PWM_RED2_Pin GPIO_PIN_1
#define PWM_RED2_GPIO_Port GPIOA
#define PWM_RED3_Pin GPIO_PIN_2
#define PWM_RED3_GPIO_Port GPIOA
#define PWM_RED4_Pin GPIO_PIN_3
#define PWM_RED4_GPIO_Port GPIOA
#define GRID_COLUMN_OUT5_Pin GPIO_PIN_4
#define GRID_COLUMN_OUT5_GPIO_Port GPIOA
#define GRID_COLUMN_OUT4_Pin GPIO_PIN_5
#define GRID_COLUMN_OUT4_GPIO_Port GPIOA
#define PWM_BLUE1_Pin GPIO_PIN_6
#define PWM_BLUE1_GPIO_Port GPIOA
#define PWM_BLUE2_Pin GPIO_PIN_7
#define PWM_BLUE2_GPIO_Port GPIOA
#define PWM_BLUE3_Pin GPIO_PIN_0
#define PWM_BLUE3_GPIO_Port GPIOB
#define PWM_BLUE4_Pin GPIO_PIN_1
#define PWM_BLUE4_GPIO_Port GPIOB
#define LCD_RESET_Pin GPIO_PIN_2
#define LCD_RESET_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_10
#define LCD_DC_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_LIGHT_Pin GPIO_PIN_14
#define LCD_LIGHT_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define MIDI_OUT_Pin GPIO_PIN_6
#define MIDI_OUT_GPIO_Port GPIOC
#define MIDI_IN_Pin GPIO_PIN_7
#define MIDI_IN_GPIO_Port GPIOC
#define MIDI_OUT_DETECT_Pin GPIO_PIN_9
#define MIDI_OUT_DETECT_GPIO_Port GPIOC
#define GRID_COLUMN_OUT1_Pin GPIO_PIN_8
#define GRID_COLUMN_OUT1_GPIO_Port GPIOA
#define GRID_COLUMN_OUT2_Pin GPIO_PIN_9
#define GRID_COLUMN_OUT2_GPIO_Port GPIOA
#define GRID_COLUMN_OUT3_Pin GPIO_PIN_10
#define GRID_COLUMN_OUT3_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWDCLK_Pin GPIO_PIN_14
#define SWDCLK_GPIO_Port GPIOA
#define GRID_COLUMN_OUT6_Pin GPIO_PIN_15
#define GRID_COLUMN_OUT6_GPIO_Port GPIOA
#define BUTTON_IN2_Pin GPIO_PIN_10
#define BUTTON_IN2_GPIO_Port GPIOC
#define ROTARY2_A_Pin GPIO_PIN_11
#define ROTARY2_A_GPIO_Port GPIOC
#define ROTARY2_B_Pin GPIO_PIN_12
#define ROTARY2_B_GPIO_Port GPIOC
#define PWM_GREEN1_Pin GPIO_PIN_6
#define PWM_GREEN1_GPIO_Port GPIOB
#define PWM_GREEN2_Pin GPIO_PIN_7
#define PWM_GREEN2_GPIO_Port GPIOB
#define PWM_GREEN3_Pin GPIO_PIN_8
#define PWM_GREEN3_GPIO_Port GPIOB
#define PWM_GREEN4_Pin GPIO_PIN_9
#define PWM_GREEN4_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
