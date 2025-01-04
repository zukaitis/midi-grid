#pragma once

#include "stm32f4xx_hal.h"

namespace mcu {

GPIO_TypeDef* const GRID_BUTTON_IN_GPIO_PORT = GPIOC;
const uint16_t BUTTON_IN1_Pin = GPIO_PIN_13;
const uint16_t BUTTON_IN2_Pin = GPIO_PIN_10;
const uint16_t ROTARY1_A_Pin = GPIO_PIN_14;
const uint16_t ROTARY1_B_Pin = GPIO_PIN_15;
const uint16_t ROTARY2_A_Pin = GPIO_PIN_11;
const uint16_t ROTARY2_B_Pin = GPIO_PIN_12;
const uint16_t GRID_BUTTON_IN1_Pin = GPIO_PIN_0;
const uint16_t GRID_BUTTON_IN2_Pin = GPIO_PIN_1;
const uint16_t GRID_BUTTON_IN3_Pin = GPIO_PIN_2;
const uint16_t GRID_BUTTON_IN4_Pin = GPIO_PIN_3;

GPIO_TypeDef* const COLUMN_OUT_GPIO_PORT = GPIOA;
const uint16_t COLUMN_OUT1_Pin = GPIO_PIN_8;
const uint16_t COLUMN_OUT2_Pin = GPIO_PIN_9;
const uint16_t COLUMN_OUT3_Pin = GPIO_PIN_10;
const uint16_t COLUMN_OUT4_Pin = GPIO_PIN_5;
const uint16_t COLUMN_OUT5_Pin = GPIO_PIN_4;
const uint16_t COLUMN_OUT6_Pin = GPIO_PIN_15;

GPIO_TypeDef* const PWM_RED_GPIO_PORT = GPIOA;
const uint16_t PWM_RED1_Pin = GPIO_PIN_0;
const uint16_t PWM_RED2_Pin = GPIO_PIN_1;
const uint16_t PWM_RED3_Pin = GPIO_PIN_2;
const uint16_t PWM_RED4_Pin = GPIO_PIN_3;

GPIO_TypeDef* const PWM_GREEN_GPIO_PORT = GPIOB;
const uint16_t PWM_GREEN1_Pin = GPIO_PIN_6;
const uint16_t PWM_GREEN2_Pin = GPIO_PIN_7;
const uint16_t PWM_GREEN3_Pin = GPIO_PIN_8;
const uint16_t PWM_GREEN4_Pin = GPIO_PIN_9;

GPIO_TypeDef* const PWM_BLUE1_2_GPIO_PORT = GPIOA;
const uint16_t PWM_BLUE1_Pin = GPIO_PIN_6;
const uint16_t PWM_BLUE2_Pin = GPIO_PIN_7;
GPIO_TypeDef* const PWM_BLUE3_4_GPIO_PORT = GPIOB;
const uint16_t PWM_BLUE3_Pin = GPIO_PIN_0;
const uint16_t PWM_BLUE4_Pin = GPIO_PIN_1;

GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
const uint16_t RESET_Pin = GPIO_PIN_2;
const uint16_t DC_Pin = GPIO_PIN_10;
const uint16_t CS_Pin = GPIO_PIN_12;
const uint16_t SCK_Pin = GPIO_PIN_13;
const uint16_t MOSI_Pin = GPIO_PIN_15;
const uint16_t LIGHT_Pin = GPIO_PIN_5;

}  // namespace mcu
