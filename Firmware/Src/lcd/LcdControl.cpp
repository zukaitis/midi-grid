#include "lcd/LcdControl.h"

#include <string.h>

namespace lcd_control
{

static DMA_HandleTypeDef lcdSpiDma;

extern "C" void DMA1_Stream4_IRQHandler()
{
    HAL_DMA_IRQHandler(&lcdSpiDma);
}

LcdControl::LcdControl()
{
}

LcdControl::~LcdControl()
{
}

void LcdControl::initialize()
{
    initializeGpio();
    initializeSpi();
    initializeDma();
    initializeBacklight();

    resetController();
    writeCommand( 0x21 ); // LCD extended commands.
    writeCommand( 0xB8 ); // set LCD Vop(Contrast).
    writeCommand( 0x04 ); // set temp coefficent.
    writeCommand( 0x15 ); // LCD bias mode 1:65. used to be 0x14 - 1:40
    writeCommand( 0x20 ); // LCD basic commands.
    writeCommand( 0x0C ); // LCD normal.
}

void LcdControl::setBacklightIntensity( uint8_t intensity )
{
    if (intensity >= NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS)
    {
        intensity = NUMBER_OF_BACKLIGHT_INTENSITY_LEVELS - 1;
    }

    const uint16_t numberOfFullySetBytes = BACKLIGHT_INTENSITY[intensity] / 8;
    const int16_t numberOfFullyResetBytes = BACKLIGHT_OUTPUT_BUFFER_SIZE - numberOfFullySetBytes - 1; // -1 is for manually set byte

    if (numberOfFullySetBytes > 0)
    {
        memset( &backlightOutputBuffer_[0], 0xFF, numberOfFullySetBytes );
    }

    if (numberOfFullySetBytes < BACKLIGHT_OUTPUT_BUFFER_SIZE)
    {
        backlightOutputBuffer_[numberOfFullySetBytes] = static_cast<uint8_t>(0xFF << (8 - (BACKLIGHT_INTENSITY[intensity] % 8)));
    }

    if (numberOfFullyResetBytes > 0)
    {
        memset( &backlightOutputBuffer_[numberOfFullySetBytes+1], 0x00, static_cast<uint16_t>(numberOfFullyResetBytes) );
    }
}

void LcdControl::transmit( uint8_t* const buffer )
{
    setCursor( 0, 0 );

    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( LCD_GPIO_Port, DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi_, &buffer[0], BUFFER_SIZE );
}

void LcdControl::initializeBacklight()
{
    __HAL_RCC_SPI3_CLK_ENABLE();

    backlightSpi_.Instance = SPI3;
    backlightSpi_.Init.Mode = SPI_MODE_MASTER;
    backlightSpi_.Init.Direction = SPI_DIRECTION_2LINES;
    backlightSpi_.Init.DataSize = SPI_DATASIZE_8BIT;
    backlightSpi_.Init.CLKPolarity = SPI_POLARITY_LOW;
    backlightSpi_.Init.CLKPhase = SPI_PHASE_1EDGE;
    backlightSpi_.Init.NSS = SPI_NSS_SOFT;
    backlightSpi_.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // 100Mhz / 128[BACKLIGHT_OUTPUT_BUFFER_SIZE] / 256 = ~3kHz
    backlightSpi_.Init.FirstBit = SPI_FIRSTBIT_MSB;
    backlightSpi_.Init.TIMode = SPI_TIMODE_DISABLE;
    backlightSpi_.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &backlightSpi_ );
    SET_BIT(backlightSpi_.Instance->CR2, SPI_CR2_TXDMAEN); // enable TX dma control bit

    __HAL_RCC_DMA1_CLK_ENABLE();

    backlightDmaConfiguration_.Instance = DMA1_Stream5;
    backlightDmaConfiguration_.Init.Channel = DMA_CHANNEL_0;
    backlightDmaConfiguration_.Init.Direction = DMA_MEMORY_TO_PERIPH;
    backlightDmaConfiguration_.Init.PeriphInc = DMA_PINC_DISABLE;
    backlightDmaConfiguration_.Init.MemInc = DMA_MINC_ENABLE;
    backlightDmaConfiguration_.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    backlightDmaConfiguration_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    backlightDmaConfiguration_.Init.Mode = DMA_CIRCULAR;
    backlightDmaConfiguration_.Init.Priority = DMA_PRIORITY_MEDIUM;
//    backlightDmaConfiguration_.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
//    backlightDmaConfiguration_.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    backlightDmaConfiguration_.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init( &backlightDmaConfiguration_ );
    __HAL_LINKDMA( &backlightSpi_, hdmatx, backlightDmaConfiguration_ );

    HAL_DMA_Start( &backlightDmaConfiguration_,
            reinterpret_cast<uint32_t>(&backlightOutputBuffer_[0]),
            reinterpret_cast<uint32_t>(&backlightSpi_.Instance->DR),
            BACKLIGHT_OUTPUT_BUFFER_SIZE );

    __HAL_SPI_ENABLE(&backlightSpi_);
}

void LcdControl::initializeDma()
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    // SPI2 DMA Init
    // SPI2_TX Init
    lcdSpiDma.Instance = DMA1_Stream4;
    lcdSpiDma.Init.Channel = DMA_CHANNEL_0;
    lcdSpiDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    lcdSpiDma.Init.PeriphInc = DMA_PINC_DISABLE;
    lcdSpiDma.Init.MemInc = DMA_MINC_ENABLE;
    lcdSpiDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    lcdSpiDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    lcdSpiDma.Init.Mode = DMA_NORMAL;
    lcdSpiDma.Init.Priority = DMA_PRIORITY_LOW;
    lcdSpiDma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init( &lcdSpiDma );

    __HAL_LINKDMA( &lcdSpi_, hdmatx, lcdSpiDma );

    // DMA interrupt init
    // DMA1_Stream4_IRQn interrupt configuration
    HAL_NVIC_SetPriority( DMA1_Stream4_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Stream4_IRQn );
}

void LcdControl::initializeGpio()
{
    GPIO_InitTypeDef gpioConfiguration;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Pin = DC_Pin | RESET_Pin;
    gpioConfiguration.Mode = GPIO_MODE_OUTPUT_PP;
    gpioConfiguration.Pull = GPIO_NOPULL;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );

    gpioConfiguration.Pin = CS_Pin | SCK_Pin | MOSI_Pin;
    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );

    gpioConfiguration.Pin = LIGHT_Pin;
    gpioConfiguration.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );
}

void LcdControl::initializeSpi()
{
    __HAL_RCC_SPI2_CLK_ENABLE();

    // SPI2 parameter configuration
    lcdSpi_.Instance = SPI2;
    lcdSpi_.Init.Mode = SPI_MODE_MASTER;
    lcdSpi_.Init.Direction = SPI_DIRECTION_2LINES;
    lcdSpi_.Init.DataSize = SPI_DATASIZE_8BIT;
    lcdSpi_.Init.CLKPolarity = SPI_POLARITY_LOW;
    lcdSpi_.Init.CLKPhase = SPI_PHASE_1EDGE;
    lcdSpi_.Init.NSS = SPI_NSS_HARD_OUTPUT;
    lcdSpi_.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 3MBbit?
    lcdSpi_.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcdSpi_.Init.TIMode = SPI_TIMODE_DISABLE;
    lcdSpi_.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &lcdSpi_ );
}

void LcdControl::resetController()
{
    HAL_GPIO_WritePin( LCD_GPIO_Port, RESET_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( LCD_GPIO_Port, RESET_Pin, GPIO_PIN_SET );
}

void LcdControl::setCursor( const uint8_t x, const uint8_t y )
{
    writeCommand( 0x80 | x ); // column.
    writeCommand( 0x40 | y ); // row.
}

void LcdControl::writeCommand( const uint8_t command )
{
    uint8_t commandToWrite = command;

    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( LCD_GPIO_Port, DC_Pin, GPIO_PIN_RESET ); //command mode
    HAL_SPI_Transmit_DMA( &lcdSpi_, &commandToWrite, 1 );
}

} // namespace

