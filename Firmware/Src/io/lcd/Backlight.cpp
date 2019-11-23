#include "io/lcd/Backlight.hpp"

#include "system/gpio_definitions.h"
#include "stm32f4xx_hal.h"

#include "ThreadConfigurations.h"
#include "ticks.hpp"

#include <cstring>

namespace lcd
{

static const uint16_t kIntensity[65] = {
        0, 1, 2, 3, 5, 8, 11, 15, 20, 25, 30, 36, 43, 50, 57, 65,
        74, 82, 92, 102, 112, 123, 135, 147, 159, 172, 185, 199, 213, 228, 243, 258,
        274, 291, 308, 325, 343, 362, 380, 400, 419, 439, 460, 481, 502, 524, 547, 570,
        593, 616, 641, 665, 690, 716, 741, 768, 795, 822, 849, 877, 906, 935, 964, 994,
        1024
};

uint32_t Backlight::outputBuffer_[kOutputBufferSize_];

static DMA_HandleTypeDef dmaConfiguration;
static SPI_HandleTypeDef spiConfiguration;

Backlight::Backlight():
    Thread( "Backlight", kBacklight.stackDepth, kBacklight.priority ),
    appointedIntensity_( 0 ),
    currentIntensity_( 0 )
{
    setMomentaryIntensity( 0 );
}

void Backlight::initialize()
{
    initializeGpio();
    initializeSpi();
    initializeDma();

    __HAL_SPI_ENABLE( &spiConfiguration ); // start SPI transmission
    Start(); // start thread
}

void Backlight::Run()
{
    static const TickType_t updatePeriod = freertos::Ticks::MsToTicks( 50 );

    while (true)
    {
        if (currentIntensity_ != appointedIntensity_)
        {
            if (currentIntensity_ > appointedIntensity_)
            {
                setMomentaryIntensity( --currentIntensity_ );
            }
            else
            {
                setMomentaryIntensity( ++currentIntensity_ );
            }

            DelayUntil( updatePeriod );
        }
        else
        {
            // block until intensity is changed
            appointedIntensityChanged_.Take();
        }
    }
}

void Backlight::setIntensity( uint8_t intensity )
{
    appointedIntensity_ = intensity;
    appointedIntensityChanged_.Give();
}

void Backlight::setMomentaryIntensity( uint8_t intensity )
{
    uint8_t wordIndex = 0;

    if (intensity > maximumIntensity)
    {
        intensity = maximumIntensity;
    }

    const uint16_t numberOfFullySetWords = kIntensity[intensity] / 32;

    while (wordIndex < numberOfFullySetWords)
    {
        //fill fully set words with binary ones
        outputBuffer_[wordIndex++] = 0xFFFFFFFF;
    }

    if (wordIndex < kOutputBufferSize_)
    {
        outputBuffer_[wordIndex++] = 0xFFFFFFFF >> (32 - (kIntensity[intensity] % 32));
    }

    while (wordIndex < kOutputBufferSize_)
    {
        // fill rest of the buffer with binary zeros
        outputBuffer_[wordIndex++] = 0x00000000;
    }
}

void Backlight::initializeDma()
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
            kOutputBufferSize_ * sizeof(outputBuffer_[0]) );
}

void Backlight::initializeGpio()
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

void Backlight::initializeSpi()
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
