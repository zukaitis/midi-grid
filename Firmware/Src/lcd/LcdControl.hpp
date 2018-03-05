/*
 * LcdConfiguration.hpp
 *
 *  Created on: 2018-03-04
 *      Author: Gedas
 */

#ifndef LCD_LCDCONTROL_HPP_
#define LCD_LCDCONTROL_HPP_

#include <stdint.h>
#include "stm32f4xx_hal.h"

class LcdControl
{
public:
    LcdControl();
    ~LcdControl();

    void initialize();
    void update(uint8_t* buffer);

    const uint16_t LCD_BUFFER_SIZE = 504;

private:

    void resetController();
    void writeCommand( const uint8_t command );

    void initializeBacklightTimer();

    void initializeDma();

    void setCursor( const uint8_t x, const uint8_t y );
    void initializeGpio();

    void initializeSpi();
    void LCD_init();
    void initializeBacklightPwm();

    TIM_HandleTypeDef lcdBacklightPwmTimer;
    SPI_HandleTypeDef lcdSpi;

    GPIO_TypeDef* LCD_GPIO_Port = GPIOB;
    const uint16_t LCD_RESET_Pin = GPIO_PIN_2;
    const uint16_t LCD_DC_Pin = GPIO_PIN_10;
    const uint16_t LCD_CS_Pin = GPIO_PIN_12;
    const uint16_t LCD_SCK_Pin = GPIO_PIN_13;
    const uint16_t LCD_LIGHT_Pin = GPIO_PIN_14;
    const uint16_t LCD_MOSI_Pin = GPIO_PIN_15;
};



#endif /* LCD_LCDCONTROL_HPP_ */
