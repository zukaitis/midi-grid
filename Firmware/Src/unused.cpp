// stuff that might come into use later

#include "main.hpp"
#include "stm32f4xx_hal.h"

#define MIDI_OUT_Pin GPIO_PIN_6
#define MIDI_OUT_GPIO_Port GPIOC
#define MIDI_IN_Pin GPIO_PIN_7
#define MIDI_IN_GPIO_Port GPIOC
#define MIDI_OUT_DETECT_Pin GPIO_PIN_9
#define MIDI_OUT_DETECT_GPIO_Port GPIOC

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart6;

void MX_USART6_UART_Init(void);
void MX_ADC1_Init(void);

/* ADC1 init function */
void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;
    static GPIO_InitTypeDef GPIO_InitStruct;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */

    __GPIOC_CLK_ENABLE();
    __ADC1_CLK_ENABLE();

    __HAL_RCC_ADC1_CLK_ENABLE();

    /**ADC1 GPIO Configuration
    PC4     ------> ADC1_IN14
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/* USART6 init function */
void MX_USART6_UART_Init(void)
{
    static GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = MIDI_OUT_Pin|MIDI_IN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    huart6.Instance = USART6;
    huart6.Init.BaudRate = 31250;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart6);
}

#if 0
uint16_t brightnessTestResult[46000];
void runBrigthnessTest();

void runBrigthnessTest()
{
    volatile uint32_t nextReadoutTime = 0;
    uint16_t result = 0;
    uint16_t i = 0;
    grid_setLedOutputDirectly(0, 0, 1001, 1001, 1001);
    grid_setLedOutputDirectly(1, 0, 1001, 47000, 47000);
    grid_setLedOutputDirectly(2, 0, 47000, 1001, 47000);
    grid_setLedOutputDirectly(3, 0, 47000, 47000, 1001);
    grid_setLedOutputDirectly(9, 4, 47000, 47000, 47000);

    nextReadoutTime = HAL_GetTick() + 10000; // 10s delay at the start
    while (HAL_GetTick() < nextReadoutTime)
    {};

    for (i=47000; i>1000;--i)
    {
        grid_setLedOutputDirectly(9, 4, i, 47000, 47000);
        nextReadoutTime = HAL_GetTick() + 10; // 10ms delay
        while (HAL_GetTick() < nextReadoutTime)
        {};
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK)
        {
            result = HAL_ADC_GetValue(&hadc1);
            brightnessTestResult[47000-i] = result;
            if (4095 == result)
            {
                break;
            }
        }
    }
}
#endif
