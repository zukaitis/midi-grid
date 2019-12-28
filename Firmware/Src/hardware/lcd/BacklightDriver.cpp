#include "hardware/lcd/BacklightDriver.h"

#include "system/gpio_definitions.h"
#include "stm32f4xx_hal.h"

namespace hardware
{

namespace lcd
{

static const etl::array<uint16_t, 65> kIntensity = {
    0, 1, 2, 3, 5, 8, 11, 15, 20, 25, 30, 36, 43, 50, 57, 65,
    74, 82, 92, 102, 112, 123, 135, 147, 159, 172, 185, 199, 213, 228, 243, 258,
    274, 291, 308, 325, 343, 362, 380, 400, 419, 439, 460, 481, 502, 524, 547, 570,
    593, 616, 641, 665, 690, 716, 741, 768, 795, 822, 849, 877, 906, 935, 964, 994,
    1024
};

etl::array<uint32_t, 32> BacklightDriver::outputBuffer_ = {};

static DMA_HandleTypeDef dmaConfiguration;
static SPI_HandleTypeDef spiConfiguration;

BacklightDriver::BacklightDriver()
{
}

BacklightDriver::~BacklightDriver()
{
}

void BacklightDriver::initialize()
{
    initializeGpio();
    initializeSpi();
    initializeDma();

    __HAL_SPI_ENABLE( &spiConfiguration ); // start SPI transmission
}

void BacklightDriver::setIntensity( const uint8_t intensity )
{
    uint8_t wordIndex = 0;

    const uint16_t numberOfFullySetWords = kIntensity.at( intensity ) / 32;

    while (wordIndex < numberOfFullySetWords)
    {
        //fill fully set words with binary ones
        outputBuffer_[wordIndex++] = 0xFFFFFFFF;
    }

    if (wordIndex < outputBuffer_.size())
    {
        outputBuffer_[wordIndex++] = 0xFFFFFFFF >> (32 - (kIntensity.at( intensity ) % 32));
    }

    while (wordIndex < outputBuffer_.size())
    {
        // fill rest of the buffer with binary zeros
        outputBuffer_[wordIndex++] = 0x00000000;
    }
}

uint8_t BacklightDriver::getMaximumIntensity() const
{
    return maximumIntensity_;
}

void BacklightDriver::initializeDma()
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    dmaConfiguration.Instance = DMA1_Stream5;
    dmaConfiguration.Init.Channel = DMA_CHANNEL_0;
    dmaConfiguration.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dmaConfiguration.Init.PeriphInc = DMA_PINC_DISABLE;
    dmaConfiguration.Init.MemInc = DMA_MINC_ENABLE;
    dmaConfiguration.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dmaConfiguration.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dmaConfiguration.Init.Mode = DMA_CIRCULAR;
    dmaConfiguration.Init.Priority = DMA_PRIORITY_MEDIUM;
    dmaConfiguration.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init( &dmaConfiguration );
    __HAL_LINKDMA( &spiConfiguration, hdmatx, dmaConfiguration );

    HAL_DMA_Start( &dmaConfiguration,
            reinterpret_cast<uint32_t>(&outputBuffer_[0]),
            reinterpret_cast<uint32_t>(&spiConfiguration.Instance->DR),
            sizeof( outputBuffer_ ) );
}

void BacklightDriver::initializeGpio()
{
    static GPIO_InitTypeDef gpioConfiguration;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Pin = mcu::LIGHT_Pin;
    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Pull = GPIO_NOPULL;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioConfiguration.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init( mcu::LCD_GPIO_Port, &gpioConfiguration );
}

void BacklightDriver::initializeSpi()
{
    __HAL_RCC_SPI3_CLK_ENABLE();

    spiConfiguration.Instance = SPI3;
    spiConfiguration.Init.Mode = SPI_MODE_MASTER;
    spiConfiguration.Init.Direction = SPI_DIRECTION_2LINES;
    spiConfiguration.Init.DataSize = SPI_DATASIZE_8BIT;
    spiConfiguration.Init.CLKPolarity = SPI_POLARITY_LOW;
    spiConfiguration.Init.CLKPhase = SPI_PHASE_1EDGE;
    spiConfiguration.Init.NSS = SPI_NSS_SOFT;
    spiConfiguration.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // 100Mhz / (4*32)[BACKLIGHT_OUTPUT_BUFFER_SIZE] / 256 = ~3kHz
    spiConfiguration.Init.FirstBit = SPI_FIRSTBIT_LSB;
    spiConfiguration.Init.TIMode = SPI_TIMODE_DISABLE;
    spiConfiguration.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init( &spiConfiguration );
    SET_BIT( spiConfiguration.Instance->CR2, SPI_CR2_TXDMAEN ); // enable TX dma control bit
}

} // namespace lcd

} // namespace hardware
