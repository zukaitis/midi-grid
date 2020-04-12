#include "hardware/lcd/Spi.h"
#include "hardware/lcd/SpiInterface.h"
#include "system/gpio_definitions.h"

#include "stm32f4xx_hal.h"

namespace hardware
{

namespace lcd
{

static DMA_HandleTypeDef lcdSpiDma;
static SPI_HandleTypeDef lcdSpi;

extern "C" void DMA1_Stream4_IRQHandler()
{
    HAL_DMA_IRQHandler( &lcdSpiDma );
}

Spi::Spi() = default;

Spi::~Spi() = default;

void Spi::initialize() const
{
    initializeGpio();
    initializeSpi();
    initializeDma();
}

void Spi::writeCommand( const uint8_t command ) const
{
    static uint8_t buffer = 0;

    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_RESET ); //command mode
    buffer = command;
    HAL_SPI_Transmit_DMA( &lcdSpi, &buffer, 1 );
}

void Spi::writeData( const uint8_t& data, const uint32_t size ) const
{
    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi, const_cast<uint8_t*>( &data ), size );
}

void Spi::writeData( const ::lcd::RawDataView& data ) const
{
    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi, const_cast<uint8_t*>(&data.front()), data.size() );
}

void Spi::writeData( const ::lcd::PixelView& data) const
{
    while (HAL_DMA_STATE_BUSY == HAL_DMA_GetState( &lcdSpiDma ))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi, const_cast<uint8_t*>(&data.front().front()), data.size() * data.at(0).size() );
}

void Spi::initializeDma() const
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

void Spi::initializeGpio() const
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

void Spi::initializeSpi() const
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
    lcdSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // 24MBbit
    lcdSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcdSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    lcdSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &lcdSpi );
}

void Spi::reset() const
{
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_SET );
}

} // namespace lcd

} // namespace hardware
