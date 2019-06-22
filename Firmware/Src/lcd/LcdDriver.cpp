#include "lcd/LcdDriver.hpp"
#include "system/gpio_definitions.h"

#include "stm32f4xx_hal.h"

namespace lcd
{

static DMA_HandleTypeDef lcdSpiDma;
static SPI_HandleTypeDef lcdSpi;

extern "C" void DMA1_Stream4_IRQHandler()
{
    HAL_DMA_IRQHandler(&lcdSpiDma);
}

LcdDriver::LcdDriver()
{
}

LcdDriver::~LcdDriver()
{
}

void LcdDriver::initialize()
{
    initializeGpio();
    initializeSpi();
    initializeDma();

    resetController();
    writeCommand( 0x21 ); // LCD extended commands.
    writeCommand( 0xB8 ); // set LCD Vop(Contrast).
    writeCommand( 0x04 ); // set temp coefficent.
    writeCommand( 0x15 ); // LCD bias mode 1:65. used to be 0x14 - 1:40
    writeCommand( 0x20 ); // LCD basic commands.
    writeCommand( 0x0C ); // LCD normal.
}

void LcdDriver::transmit( uint8_t* const buffer )
{
    setCursor( 0, 0 );

    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi, &buffer[0], bufferSize );
}

void LcdDriver::initializeDma()
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

    __HAL_LINKDMA( &lcdSpi, hdmatx, lcdSpiDma );

    // DMA interrupt init
    // DMA1_Stream4_IRQn interrupt configuration
    HAL_NVIC_SetPriority( DMA1_Stream4_IRQn, 6, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Stream4_IRQn );
}

void LcdDriver::initializeGpio()
{
    static GPIO_InitTypeDef gpioConfiguration;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Pin = mcu::DC_Pin | mcu::RESET_Pin;
    gpioConfiguration.Mode = GPIO_MODE_OUTPUT_PP;
    gpioConfiguration.Pull = GPIO_NOPULL;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( mcu::LCD_GPIO_Port, &gpioConfiguration );

    gpioConfiguration.Pin = mcu::CS_Pin | mcu::SCK_Pin | mcu::MOSI_Pin;
    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init( mcu::LCD_GPIO_Port, &gpioConfiguration );
}

void LcdDriver::initializeSpi()
{
    __HAL_RCC_SPI2_CLK_ENABLE();

    // SPI2 parameter configuration
    lcdSpi.Instance = SPI2;
    lcdSpi.Init.Mode = SPI_MODE_MASTER;
    lcdSpi.Init.Direction = SPI_DIRECTION_2LINES;
    lcdSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    lcdSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    lcdSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    lcdSpi.Init.NSS = SPI_NSS_HARD_OUTPUT;
    lcdSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 3MBbit?
    lcdSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcdSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    lcdSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &lcdSpi );
}

void LcdDriver::resetController()
{
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_SET );
}

void LcdDriver::setCursor( const uint8_t x, const uint8_t y )
{
    writeCommand( 0x80 | x ); // column.
    writeCommand( 0x40 | y ); // row.
}

void LcdDriver::writeCommand( const uint8_t command )
{
    uint8_t commandToWrite = command;

    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_RESET ); //command mode
    HAL_SPI_Transmit_DMA( &lcdSpi, &commandToWrite, 1 );
}

} // namespace lcd

