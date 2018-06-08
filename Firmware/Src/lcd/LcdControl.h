#ifndef LCD_LCDCONTROL_H_
#define LCD_LCDCONTROL_H_

#include "stm32f4xx_hal.h"

namespace lcd_control
{

static GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
static const uint16_t RESET_Pin = GPIO_PIN_2;
static const uint16_t DC_Pin = GPIO_PIN_10;
static const uint16_t CS_Pin = GPIO_PIN_12;
static const uint16_t SCK_Pin = GPIO_PIN_13;
static const uint16_t LIGHT_Pin = GPIO_PIN_14;
static const uint16_t MOSI_Pin = GPIO_PIN_15;

static TIM_TypeDef* const BACKLIGHT_TIMER = TIM10; //TIM1;
static const uint8_t NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS = 65;
static const uint16_t BUFFER_SIZE = 504;

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
    void setBacklightIntensity( uint8_t intensity );
    void transmit( uint8_t* const buffer );

private:

    void initializeBacklight();
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    void resetController();
    void setCursor( const uint8_t x, const uint8_t y );
    void writeCommand( const uint8_t command );

    TIM_HandleTypeDef lcdBacklightPwmTimer_;
    SPI_HandleTypeDef lcdSpi_;
};

} // namespace

#endif // LCD_LCDCONTROL_H_
