#include "hardware/lcd/Spi.h"
#include "hardware/lcd/SpiInterface.h"
#include "stm32f4xx_hal_spi.h"
#include "system/gpio_definitions.h"

namespace hardware
{

namespace lcd
{

static DMA_HandleTypeDef dmaHandle;
static SPI_HandleTypeDef spiHandle;

extern "C" void DMA1_Stream4_IRQHandler()
{
    HAL_DMA_IRQHandler( &dmaHandle );
}

bool Spi::bulkTransferInProgress_ = false;
uint16_t Spi::bulkTransferStartAddressIncrement_ = 0;
const uint8_t* Spi::bulkTransferStartAddress_ = nullptr;
uint16_t Spi::bulkTransferSegmentSize_ = 0;
uint16_t Spi::bulkTransferSegmentsLeft_ = 0;

Spi::Spi() = default;

Spi::~Spi() = default;

void Spi::bulkTransferPartComplete( __DMA_HandleTypeDef* /*unused*/ )
{
    bulkTransferSegmentsLeft_--;

    if (0 == bulkTransferSegmentsLeft_)
    {
        // transmit done
        bulkTransferInProgress_ = false;
        disableTransferCompleteCallback();
    }
    else
    {
        bulkTransferStartAddress_ += bulkTransferStartAddressIncrement_;
        HAL_SPI_Transmit_DMA( &spiHandle, const_cast<uint8_t*>(bulkTransferStartAddress_), bulkTransferSegmentSize_);
    }
}

void Spi::initialize() const
{
    initializeGpio();
    initializeSpi();
    initializeDma();
}

void Spi::setBulkTransferStartAddressIncrement( const uint16_t increment )
{
    bulkTransferStartAddressIncrement_ = increment;
}

void Spi::writeCommand( const uint8_t command ) const
{
    static uint8_t buffer = 0;

    waitWhileSpiIsBusy();
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_RESET ); //command mode
    buffer = command;
    HAL_SPI_Transmit_DMA( &spiHandle, &buffer, 1 );
}

void Spi::writeData( const uint8_t& data, const uint32_t size ) const
{
    waitWhileSpiIsBusy();
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &spiHandle, const_cast<uint8_t*>( &data ), size );
}

void Spi::writeData( const RawDataView& data ) const
{
    waitWhileSpiIsBusy();
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &spiHandle, const_cast<uint8_t*>(&data.front()), data.size() );
}

void Spi::writeData( const void* const address, const uint16_t size ) const
{
    waitWhileSpiIsBusy();
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::DC_Pin, GPIO_PIN_SET );  //data mode
    HAL_SPI_Transmit_DMA( &spiHandle, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(address)), size );
}

void Spi::writeData( const void* const startAddress, uint16_t segmentSize, const uint16_t numberOfSegments ) const
{
    waitWhileSpiIsBusy();

    bulkTransferInProgress_ = true;
    bulkTransferStartAddress_ = reinterpret_cast<const uint8_t*>(&startAddress);
    bulkTransferSegmentSize_ = segmentSize;
    bulkTransferSegmentsLeft_ = numberOfSegments;

    enableTransferCompleteCallback();
    HAL_SPI_Transmit_DMA( &spiHandle, const_cast<uint8_t*>(bulkTransferStartAddress_), bulkTransferSegmentSize_);
}

void Spi::initializeDma()
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    // SPI2 DMA Init
    // SPI2_TX Init
    dmaHandle.Instance = DMA1_Stream4;
    dmaHandle.Init.Channel = DMA_CHANNEL_0;
    dmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    dmaHandle.Init.MemInc = DMA_MINC_ENABLE;
    dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dmaHandle.Init.Mode = DMA_NORMAL;
    dmaHandle.Init.Priority = DMA_PRIORITY_LOW;
    dmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init( &dmaHandle );

    __HAL_LINKDMA( &spiHandle, hdmatx, dmaHandle );

    // DMA interrupt init
    // DMA1_Stream4_IRQn interrupt configuration
    HAL_NVIC_SetPriority( DMA1_Stream4_IRQn, 6, 0 );
    HAL_NVIC_EnableIRQ( DMA1_Stream4_IRQn );
}

void Spi::initializeGpio()
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

void Spi::initializeSpi()
{
    __HAL_RCC_SPI2_CLK_ENABLE();

    // SPI2 parameter configuration
    spiHandle.Instance = SPI2;
    spiHandle.Init.Mode = SPI_MODE_MASTER;
    spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    spiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    spiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    spiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    spiHandle.Init.NSS = SPI_NSS_HARD_OUTPUT;
    spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // 48MBbit
    spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &spiHandle );
}

void Spi::reset() const
{
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( mcu::LCD_GPIO_Port, mcu::RESET_Pin, GPIO_PIN_SET );
}

void Spi::waitWhileSpiIsBusy()
{
    while ((spiHandle.State != HAL_SPI_STATE_READY) || bulkTransferInProgress_)
    {
        // wait until previous transfer is done
    }
}

void Spi::enableTransferCompleteCallback()
{
    dmaHandle.XferCpltCallback = bulkTransferPartComplete;
}

void Spi::disableTransferCompleteCallback()
{
    dmaHandle.XferCpltCallback = nullptr;
}

} // namespace lcd

} // namespace hardware
