/*
 * LcdConfiguration.cpp
 *
 *  Created on: 2018-03-04
 *      Author: Gedas
 */
#include <lcd/LcdControl.h>

namespace lcd_control
{

static DMA_HandleTypeDef lcdSpiDma;

LcdControl::LcdControl()
{
}

LcdControl::~LcdControl()
{
}

void LcdControl::resetController()
{
    HAL_GPIO_WritePin( LCD_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( LCD_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET );
}

void LcdControl::writeCommand( const uint8_t command )
{
    uint8_t commandToWrite = command;

    while(HAL_DMA_STATE_BUSY == HAL_DMA_GetState(&lcdSpiDma))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin( LCD_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET ); //command mode
    HAL_SPI_Transmit_DMA(&lcdSpi, &commandToWrite, 1);
}

void LcdControl::update(uint8_t* buffer)
{
    setCursor( 0, 0 );

    while(HAL_DMA_STATE_BUSY == HAL_DMA_GetState(&lcdSpiDma))
    {
        // wait until previous transfer is done
    }

    HAL_GPIO_WritePin(LCD_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);  //data mode
    HAL_SPI_Transmit_DMA( &lcdSpi, buffer, LCD_BUFFER_SIZE );
}

void LcdControl::setCursor( const uint8_t x, const uint8_t y )
{
    writeCommand(0x80 | x); // column.
    writeCommand(0x40 | y); // row.
}

void LcdControl::initializeBacklightTimer()
{
    HAL_GPIO_WritePin( LCD_GPIO_Port, LCD_LIGHT_Pin, GPIO_PIN_SET );
}

void LcdControl::initializeDma()
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    /* SPI2 DMA Init */
    /* SPI2_TX Init */
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
    HAL_DMA_Init(&lcdSpiDma);

    __HAL_LINKDMA( &lcdSpi, hdmatx, lcdSpiDma );

    /* DMA interrupt init */
    /* DMA1_Stream4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}

void LcdControl::initializeGpio()
{
    GPIO_InitTypeDef gpioConfiguration;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB12     ------> SPI2_NSS
    PB13     ------> SPI2_SCK
    PB15     ------> SPI2_MOSI
    */
    gpioConfiguration.Pin = LCD_DC_Pin|LCD_RESET_Pin|LCD_LIGHT_Pin;
    gpioConfiguration.Mode = GPIO_MODE_OUTPUT_PP;
    gpioConfiguration.Pull = GPIO_NOPULL;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );

    gpioConfiguration.Pin = LCD_CS_Pin|LCD_SCK_Pin|LCD_MOSI_Pin;
    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );

    gpioConfiguration.Pin = LCD_LIGHT_Pin;
    gpioConfiguration.Alternate = GPIO_AF1_TIM1;
    //HAL_GPIO_Init( LCD_GPIO_Port, &gpioConfiguration );
}

void LcdControl::initializeSpi()
{
    __HAL_RCC_SPI2_CLK_ENABLE();

    /* SPI2 parameter configuration*/
    lcdSpi.Instance = SPI2;
    lcdSpi.Init.Mode = SPI_MODE_MASTER;
    lcdSpi.Init.Direction = SPI_DIRECTION_2LINES;
    lcdSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    lcdSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    lcdSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    lcdSpi.Init.NSS = SPI_NSS_HARD_OUTPUT;
    lcdSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // 6MBbit?
    lcdSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcdSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    lcdSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    //lcdSpi.Init.CRCPolynomial = 10;
    HAL_SPI_Init(&lcdSpi);
}

void LcdControl::initialize()
{
    initializeGpio();
    initializeSpi();
    initializeDma();
    initializeBacklightTimer();

    resetController();
    writeCommand( 0x21 ); //LCD extended commands.
    writeCommand( 0xB8 ); //set LCD Vop(Contrast).
    writeCommand( 0x04 ); //set temp coefficent.
    writeCommand( 0x14 ); //LCD bias mode 1:40.
    writeCommand( 0x20 ); //LCD basic commands.
    writeCommand( 0x0C ); //LCD normal.
}

void LcdControl::initializeBacklightPwm()
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_SlaveConfigTypeDef sSlaveConfig;
    TIM_OC_InitTypeDef sConfigOC;

    __HAL_RCC_TIM1_CLK_ENABLE();

    // prescaler and period of PWM timer are calculated based on period of base timer
    lcdBacklightPwmTimer.Instance = TIM1;
    lcdBacklightPwmTimer.Init.Prescaler = 16 - 1; //~100Hz
    lcdBacklightPwmTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    lcdBacklightPwmTimer.Init.Period = 65535 - 1;
    lcdBacklightPwmTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&lcdBacklightPwmTimer);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&lcdBacklightPwmTimer, &sClockSourceConfig);

    HAL_TIM_PWM_Init(&lcdBacklightPwmTimer);

//  //sSlaveConfig.InputTrigger = TIM_TS_ITR0;
//  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
//  //sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
//  HAL_TIM_SlaveConfigSynchronization(&lcdBacklightPwmTimer, &sSlaveConfig);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 25000;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&lcdBacklightPwmTimer, &sConfigOC, TIM_CHANNEL_3);

    //TIM_CCxNChannelCmd(lcdBacklightPwmTimer.Instance, TIM_CHANNEL_3, TIM_CCxN_ENABLE);
    __HAL_TIM_ENABLE(&lcdBacklightPwmTimer);
}

extern "C" void DMA1_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&lcdSpiDma);
}

} // namespace

