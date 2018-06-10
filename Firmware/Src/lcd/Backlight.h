#ifndef LCD_BACKLIGHT_H_
#define LCD_BACKLIGHT_H_

#include "stm32f4xx_hal.h"

namespace lcd
{
namespace backlight
{

static GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
static const uint16_t LIGHT_Pin = GPIO_PIN_5;

static const uint16_t OUTPUT_BUFFER_SIZE = 32; // 10 bit resolution
static const uint8_t NUMBER_OF_INTENSITY_LEVELS = 65;

static const uint16_t INTENSITY[NUMBER_OF_INTENSITY_LEVELS] = {
        0, 1, 2, 3, 5, 8, 11, 15, 20, 25, 30, 36, 43, 50, 57, 65,
        74, 82, 92, 102, 112, 123, 135, 147, 159, 172, 185, 199, 213, 228, 243, 258,
        274, 291, 308, 325, 343, 362, 380, 400, 419, 439, 460, 481, 502, 524, 547, 570,
        593, 616, 641, 665, 690, 716, 741, 768, 795, 822, 849, 877, 906, 935, 964, 994,
        1024
};

class Backlight
{
public:
    Backlight();
    ~Backlight();

    void initialize();
    void setIntensity( uint8_t intensity );

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    uint32_t outputBuffer_[OUTPUT_BUFFER_SIZE];

    DMA_HandleTypeDef dmaConfiguration_;
    SPI_HandleTypeDef spiConfiguration_;
};

} // namespace backlight
} // namespace lcd

#endif // LCD_BACKLIGHT_H_
