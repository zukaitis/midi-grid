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
static const uint16_t LIGHT_Pin = GPIO_PIN_5;
static const uint16_t MOSI_Pin = GPIO_PIN_15;

static const uint32_t BACKLIGHT_OUTPUT_BUFFER_SIZE = 128; // 10 bit resolution
static const uint8_t NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS = 65;
static const uint16_t BUFFER_SIZE = 504;

static const uint16_t BACKLIGHT_INTENSITY[NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS] = {
        0, 1, 2, 3, 5, 8, 11, 15, 20, 25, 30, 36, 43, 50, 57, 65,
        74, 82, 92, 102, 112, 123, 135, 147, 159, 172, 185, 199, 213, 228, 243, 258,
        274, 291, 308, 325, 343, 362, 380, 400, 419, 439, 460, 481, 502, 524, 547, 570,
        593, 616, 641, 665, 690, 716, 741, 768, 795, 822, 849, 877, 906, 935, 964, 994,
        1024
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

    uint8_t backlightOutputBuffer_[BACKLIGHT_OUTPUT_BUFFER_SIZE];

    DMA_HandleTypeDef backlightDmaConfiguration_;
    SPI_HandleTypeDef backlightSpi_;
    SPI_HandleTypeDef lcdSpi_;
};

} // namespace

#endif // LCD_LCDCONTROL_H_
