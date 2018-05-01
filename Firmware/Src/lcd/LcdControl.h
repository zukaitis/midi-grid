/*
 * LcdConfiguration.hpp
 *
 *  Created on: 2018-03-04
 *      Author: Gedas
 */

#ifndef LCD_LCDCONTROL_HPP_
#define LCD_LCDCONTROL_HPP_

//#include <stdint.h>
#include "stm32f4xx_hal.h"

namespace lcd_control
{

static GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
static const uint16_t LCD_RESET_Pin = GPIO_PIN_2;
static const uint16_t LCD_DC_Pin = GPIO_PIN_10;
static const uint16_t LCD_CS_Pin = GPIO_PIN_12;
static const uint16_t LCD_SCK_Pin = GPIO_PIN_13;
static const uint16_t LCD_LIGHT_Pin = GPIO_PIN_14;
static const uint16_t LCD_MOSI_Pin = GPIO_PIN_15;

static TIM_TypeDef* const BACKLIGHT_TIMER = TIM1;

static const uint8_t NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS = 65;

static const uint16_t backlightIntensity[NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS] = {
        0, 1, 6, 17, 37, 67, 110, 167, 239, 328, 436, 564, 714, 886, 1082, 1304,
        1552, 1828, 2133, 2468, 2835, 3234, 3667, 4135, 4638, 5179, 5757, 6375, 7033, 7731, 8473, 9257,
        10085, 10959, 11879, 12846, 13861, 14926, 16040, 17205, 18423, 19693, 21017, 22395, 23829, 25320, 26868, 28474,
        30140, 31865, 33652, 35500, 37411, 39385, 41424, 43528, 45698, 47935, 50239, 52612, 55055, 57568, 60151, 62807,
        65535
};

class LcdControl
{
public:
    LcdControl();
    ~LcdControl();

    void initialize();
    void update(uint8_t* buffer);

    void setBacklightIntensity( uint8_t intensity );

    const uint16_t LCD_BUFFER_SIZE = 504;

private:

    void resetController();
    void writeCommand( const uint8_t command );

    void initializeDma();

    void setCursor( const uint8_t x, const uint8_t y );
    void initializeGpio();

    void initializeSpi();
    void LCD_init();
    void initializeBacklightPwm();

    TIM_HandleTypeDef lcdBacklightPwmTimer;
    SPI_HandleTypeDef lcdSpi;
};

} // namespace

#endif /* LCD_LCDCONTROL_HPP_ */
