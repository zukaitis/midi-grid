#pragma once

#include "stm32f4xx_hal.h"

namespace mcu {

static GPIO_TypeDef* const GRID_BUTTON_IN_GPIO_PORT = GPIOC;
static const uint16_t BUTTON_IN1_Pin = GPIO_PIN_13;
static const uint16_t BUTTON_IN2_Pin = GPIO_PIN_10;
static const uint16_t ROTARY1_A_Pin = GPIO_PIN_14;
static const uint16_t ROTARY1_B_Pin = GPIO_PIN_15;
static const uint16_t ROTARY2_A_Pin = GPIO_PIN_11;
static const uint16_t ROTARY2_B_Pin = GPIO_PIN_12;
static const uint16_t GRID_BUTTON_IN1_Pin = GPIO_PIN_0;
static const uint16_t GRID_BUTTON_IN2_Pin = GPIO_PIN_1;
static const uint16_t GRID_BUTTON_IN3_Pin = GPIO_PIN_2;
static const uint16_t GRID_BUTTON_IN4_Pin = GPIO_PIN_3;

static GPIO_TypeDef* const COLUMN_OUT_GPIO_PORT = GPIOA;
static const uint16_t COLUMN_OUT1_Pin = GPIO_PIN_8;
static const uint16_t COLUMN_OUT2_Pin = GPIO_PIN_9;
static const uint16_t COLUMN_OUT3_Pin = GPIO_PIN_10;
static const uint16_t COLUMN_OUT4_Pin = GPIO_PIN_5;
static const uint16_t COLUMN_OUT5_Pin = GPIO_PIN_4;
static const uint16_t COLUMN_OUT6_Pin = GPIO_PIN_15;

static GPIO_TypeDef* const PWM_RED_GPIO_PORT = GPIOA;
static const uint16_t PWM_RED1_Pin = GPIO_PIN_0;
static const uint16_t PWM_RED2_Pin = GPIO_PIN_1;
static const uint16_t PWM_RED3_Pin = GPIO_PIN_2;
static const uint16_t PWM_RED4_Pin = GPIO_PIN_3;

static GPIO_TypeDef* const PWM_GREEN_GPIO_PORT = GPIOB;
const uint16_t PWM_GREEN1_Pin = GPIO_PIN_6;
const uint16_t PWM_GREEN2_Pin = GPIO_PIN_7;
const uint16_t PWM_GREEN3_Pin = GPIO_PIN_8;
const uint16_t PWM_GREEN4_Pin = GPIO_PIN_9;

static GPIO_TypeDef* const PWM_BLUE1_2_GPIO_PORT = GPIOA;
static const uint16_t PWM_BLUE1_Pin = GPIO_PIN_6;
static const uint16_t PWM_BLUE2_Pin = GPIO_PIN_7;
static GPIO_TypeDef* const PWM_BLUE3_4_GPIO_PORT = GPIOB;
static const uint16_t PWM_BLUE3_Pin = GPIO_PIN_0;
static const uint16_t PWM_BLUE4_Pin = GPIO_PIN_1;

static GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
static const uint16_t RESET_Pin = GPIO_PIN_2;
static const uint16_t DC_Pin = GPIO_PIN_10;
static const uint16_t CS_Pin = GPIO_PIN_12;
static const uint16_t SCK_Pin = GPIO_PIN_13;
static const uint16_t MOSI_Pin = GPIO_PIN_15;
static const uint16_t LIGHT_Pin = GPIO_PIN_5;

}  // namespace mcu
