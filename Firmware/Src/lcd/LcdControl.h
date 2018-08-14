#ifndef LCD_LCDCONTROL_H_
#define LCD_LCDCONTROL_H_

#include "stm32f4xx_hal.h"

namespace lcd
{
namespace lcd_control
{

static GPIO_TypeDef* const LCD_GPIO_Port = GPIOB;
static const uint16_t RESET_Pin = GPIO_PIN_2;
static const uint16_t DC_Pin = GPIO_PIN_10;
static const uint16_t CS_Pin = GPIO_PIN_12;
static const uint16_t SCK_Pin = GPIO_PIN_13;

static const uint16_t MOSI_Pin = GPIO_PIN_15;


static const uint16_t BUFFER_SIZE = 504;

class LcdControl
{
public:
    LcdControl();
    ~LcdControl();

    void initialize();
    void transmit( uint8_t* const buffer );

    static DMA_HandleTypeDef lcdSpiDma_;

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    void resetController();
    void setCursor( const uint8_t x, const uint8_t y );
    void writeCommand( const uint8_t command );

    static SPI_HandleTypeDef lcdSpi_;
};

} // namespace lcd_control
} // namespace lcd

#endif // LCD_LCDCONTROL_H_
