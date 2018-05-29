/*
 * GridControl.cpp
 *
 *  Created on: 2018-03-06
 *      Author: Gedas
 */

#include "grid/GridControl.h"

namespace grid_control
{

static DMA_HandleTypeDef buttonInputDmaConfiguration;

GridControl::GridControl()
{
    turnAllLedsOff();
}

GridControl::~GridControl()
{
}

extern "C" void inputReadoutToMemory0CompleteCallbackWrapper(__DMA_HandleTypeDef * hdma)
{
    static GridControl& gridControl = GridControl::getInstance();
    gridControl.inputReadoutToMemory0CompleteCallback();
}

extern "C" void inputReadoutToMemory1CompleteCallbackWrapper(__DMA_HandleTypeDef * hdma)
{
    static GridControl& gridControl = GridControl::getInstance();
    gridControl.inputReadoutToMemory1CompleteCallback();
}

extern "C" void dmaErrorCallback(__DMA_HandleTypeDef * hdma) // unused
{
}

void GridControl::setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, bool directLed, const Colour colour )
{
    ledPositionX = (ledPositionX + NUMBER_OF_COLUMNS - TIMER_FRAME_OFFSET) % NUMBER_OF_COLUMNS;

    if (directLed)
    {
        ledOutput.Red[ledPositionX][ledPositionY] = brightnessDirect[colour.Red];
        ledOutput.Green[ledPositionX][ledPositionY] = brightnessDirect[colour.Green];
        ledOutput.Blue[ledPositionX][ledPositionY] = brightnessDirect[colour.Blue];
    }
    else
    {
        ledOutput.Red[ledPositionX][ledPositionY] = brightnessPad[colour.Red];
        ledOutput.Green[ledPositionX][ledPositionY] = brightnessPad[colour.Green];
        ledOutput.Blue[ledPositionX][ledPositionY] = brightnessPad[colour.Blue];
    }
}

void GridControl::turnAllLedsOff()
{
    uint8_t x, y;
    for (x = 0; x < NUMBER_OF_COLUMNS; x++)
    {
        for (y = 0; y < NUMBER_OF_ROWS; y++)
        {
            ledOutput.Red[x][y] = PWM_CLOCK_PERIOD; //LED_PASSIVE;
            ledOutput.Green[x][y] = PWM_CLOCK_PERIOD;
            ledOutput.Blue[x][y] = PWM_CLOCK_PERIOD;
        }
    }
}

bool GridControl::isGridColumnInputStable(const uint8_t column) const
{
    return (0 == (GRID_BUTTON_MASK & (buttonInput[0][column] ^ buttonInput[1][column])));
}

uint8_t GridControl::getGridColumnInput(const uint8_t column) const
{
    return static_cast<uint8_t>(GRID_BUTTON_MASK & buttonInput[currentlyStableInputBuffer_][column]);
}

bool GridControl::isButtonInputStable(const uint8_t button) const
{
    return (0 == (BUTTON_MASK[button] & (buttonInput[0][0] ^ buttonInput[1][0])));
}

bool GridControl::getButtonInput(const uint8_t button) const
{
    return (0 != (BUTTON_MASK[button] & buttonInput[currentlyStableInputBuffer_][0]));
}

uint8_t GridControl::getRotaryEncodersInput(const uint8_t encoder, uint8_t step) const
{
    step *= 2;
    return (ROTARY_ENCODER_MASK[encoder] & buttonInput[currentlyStableInputBuffer_][step])>>ROTARY_ENCODER_SHIFT[encoder];
}

void GridControl::initialize()
{
    initializeGpio();
    initializePwmOutputs();
    initializeBaseTimer();
    initializeDma();
}

void GridControl::initializeBaseTimer()
{
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_MasterConfigTypeDef timerMasterConfiguration;

    __HAL_RCC_TIM1_CLK_ENABLE(); //__HAL_RCC_TIM10_CLK_ENABLE();

    baseInterruptTimer.Instance = BASE_INTERRUPT_TIMER;
    baseInterruptTimer.Init.Prescaler = BASE_INTERRUPT_CLOCK_PRESCALER - 1; // 1us is the desired timer step
    baseInterruptTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    baseInterruptTimer.Init.Period = BASE_INTERRUPT_CLOCK_PERIOD - 1;  // 1s / (20*100) = 500us
    baseInterruptTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &baseInterruptTimer );

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource( &baseInterruptTimer, &timerClockSourceConfiguration );

    timerMasterConfiguration.MasterOutputTrigger = TIM_TRGO_UPDATE;
    timerMasterConfiguration.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization( &baseInterruptTimer, &timerMasterConfiguration );

    // experimental shit
    TIM_OC_InitTypeDef timerOutputCompareConfiguration;
    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_ACTIVE;

    timerOutputCompareConfiguration.Pulse = 1;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );

    timerOutputCompareConfiguration.Pulse = (BASE_INTERRUPT_CLOCK_PERIOD * 7) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );

    timerOutputCompareConfiguration.Pulse = (BASE_INTERRUPT_CLOCK_PERIOD * 8) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );

    timerOutputCompareConfiguration.Pulse = (BASE_INTERRUPT_CLOCK_PERIOD * 9) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC1 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC2 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC3 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC4 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_UPDATE );
}

void GridControl::initializeDma()
{
    DMA_InitTypeDef ledOutputDmaInitConfiguration;

    static DMA_HandleTypeDef pwmOutputRedDmaConfiguration;
    static DMA_HandleTypeDef pwmOutputGreenDmaConfiguration;
    static DMA_HandleTypeDef pwmOutputBlueDmaConfiguration;
    static DMA_HandleTypeDef columnSelectDmaConfiguration;

    __HAL_RCC_DMA2_CLK_ENABLE();

    columnSelectDmaConfiguration.Instance = DMA2_Stream1;
    columnSelectDmaConfiguration.Init.Channel = DMA_CHANNEL_6;
    columnSelectDmaConfiguration.Init.Direction = DMA_MEMORY_TO_PERIPH;
    columnSelectDmaConfiguration.Init.PeriphInc = DMA_PINC_DISABLE;
    columnSelectDmaConfiguration.Init.MemInc = DMA_MINC_ENABLE;
    columnSelectDmaConfiguration.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    columnSelectDmaConfiguration.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    columnSelectDmaConfiguration.Init.Mode = DMA_CIRCULAR;
    columnSelectDmaConfiguration.Init.Priority = DMA_PRIORITY_HIGH;
    columnSelectDmaConfiguration.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    columnSelectDmaConfiguration.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    columnSelectDmaConfiguration.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init( &columnSelectDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC1], columnSelectDmaConfiguration );
    HAL_DMA_Start( &columnSelectDmaConfiguration,
            reinterpret_cast<uint32_t>( &columnSelectValue[0] ),
            reinterpret_cast<uint32_t>( &GRID_COLUMN_CONTROL_GPIO_PORT->ODR ),
            20 );

    ledOutputDmaInitConfiguration.Channel = DMA_CHANNEL_6;
    ledOutputDmaInitConfiguration.Direction = DMA_MEMORY_TO_PERIPH;
    ledOutputDmaInitConfiguration.PeriphInc = DMA_PINC_DISABLE;
    ledOutputDmaInitConfiguration.MemInc = DMA_MINC_ENABLE;
    ledOutputDmaInitConfiguration.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    ledOutputDmaInitConfiguration.MemDataAlignment = DMA_MDATAALIGN_WORD;
    ledOutputDmaInitConfiguration.Mode = DMA_CIRCULAR; //DMA_NORMAL;
    ledOutputDmaInitConfiguration.Priority = DMA_PRIORITY_HIGH;
    ledOutputDmaInitConfiguration.FIFOMode = DMA_FIFOMODE_ENABLE;
    ledOutputDmaInitConfiguration.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    ledOutputDmaInitConfiguration.PeriphBurst = DMA_PBURST_INC4;

    pwmOutputRedDmaConfiguration.Instance = DMA2_Stream2;
    pwmOutputRedDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputRedDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC2], pwmOutputRedDmaConfiguration );
    HAL_DMA_Start( &pwmOutputRedDmaConfiguration,
            reinterpret_cast<uint32_t>(&ledOutput.Red[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_RED->DMAR),
            80 );

    pwmOutputGreenDmaConfiguration.Instance = DMA2_Stream6;
    pwmOutputGreenDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init(&pwmOutputGreenDmaConfiguration);
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC3], pwmOutputGreenDmaConfiguration );
    HAL_DMA_Start( &pwmOutputGreenDmaConfiguration,
            reinterpret_cast<uint32_t>(&ledOutput.Green[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_GREEN->DMAR),
            80 );

    pwmOutputBlueDmaConfiguration.Instance = DMA2_Stream4;
    pwmOutputBlueDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputBlueDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC4], pwmOutputBlueDmaConfiguration );
    HAL_DMA_Start( &pwmOutputBlueDmaConfiguration,
            reinterpret_cast<uint32_t>(&ledOutput.Blue[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_BLUE->DMAR),
            80 );

    buttonInputDmaConfiguration.Instance = DMA2_Stream5;
    buttonInputDmaConfiguration.Init.Channel = DMA_CHANNEL_6;
    buttonInputDmaConfiguration.Init.Direction = DMA_PERIPH_TO_MEMORY;
    buttonInputDmaConfiguration.Init.PeriphInc = DMA_PINC_DISABLE;
    buttonInputDmaConfiguration.Init.MemInc = DMA_MINC_ENABLE;
    buttonInputDmaConfiguration.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    buttonInputDmaConfiguration.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    buttonInputDmaConfiguration.Init.Mode = DMA_CIRCULAR;
    buttonInputDmaConfiguration.Init.Priority = DMA_PRIORITY_HIGH;
    buttonInputDmaConfiguration.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    buttonInputDmaConfiguration.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    buttonInputDmaConfiguration.Init.PeriphBurst = DMA_PBURST_SINGLE;
    buttonInputDmaConfiguration.XferCpltCallback = &inputReadoutToMemory0CompleteCallbackWrapper;
    buttonInputDmaConfiguration.XferM1CpltCallback = &inputReadoutToMemory1CompleteCallbackWrapper;
    buttonInputDmaConfiguration.XferErrorCallback = &dmaErrorCallback;
    HAL_DMA_Init( &buttonInputDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_UPDATE], buttonInputDmaConfiguration );

    HAL_NVIC_SetPriority( DMA2_Stream5_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( DMA2_Stream5_IRQn );

    HAL_DMAEx_MultiBufferStart_IT( &buttonInputDmaConfiguration,
            reinterpret_cast<uint32_t>( &GRID_BUTTON_IN_GPIO_PORT->IDR),
            reinterpret_cast<uint32_t>(&buttonInput[0][0]),
            reinterpret_cast<uint32_t>(&buttonInput[1][0]),
            20 );
}

void GridControl::initializeGpio()
{
      GPIO_InitTypeDef gpioConfiguration;

      // GPIO Ports Clock Enable
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      // Configure GPIO pin Output Level
      HAL_GPIO_WritePin( GRID_COLUMN_CONTROL_GPIO_PORT,
              GRID_COLUMN_OUT1_Pin | GRID_COLUMN_OUT2_Pin | GRID_COLUMN_OUT3_Pin |
              GRID_COLUMN_OUT4_Pin |GRID_COLUMN_OUT5_Pin | GRID_COLUMN_OUT6_Pin,
              GPIO_PIN_SET );

      gpioConfiguration.Pin = BUTTON_IN1_Pin | BUTTON_IN2_Pin | ROTARY1_A_Pin | ROTARY1_B_Pin | ROTARY2_A_Pin| ROTARY2_B_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLUP;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = GRID_BUTTON_IN1_Pin | GRID_BUTTON_IN2_Pin | GRID_BUTTON_IN3_Pin | GRID_BUTTON_IN4_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLDOWN;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = GRID_COLUMN_OUT1_Pin | GRID_COLUMN_OUT2_Pin | GRID_COLUMN_OUT3_Pin | GRID_COLUMN_OUT4_Pin |
              GRID_COLUMN_OUT5_Pin | GRID_COLUMN_OUT6_Pin;
      gpioConfiguration.Mode = GPIO_MODE_OUTPUT_OD;
      gpioConfiguration.Pull = GPIO_NOPULL;
      gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      HAL_GPIO_Init( GRID_COLUMN_CONTROL_GPIO_PORT, &gpioConfiguration );
}

void GridControl::initializePwmOutputs()
{
    GPIO_InitTypeDef gpioConfiguration;
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_SlaveConfigTypeDef timerSlaveConfiguration;
    TIM_OC_InitTypeDef timerOutputCompareConfiguration;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Same configuration for all three channels
    timerSlaveConfiguration.SlaveMode = TIM_SLAVEMODE_RESET;
    timerSlaveConfiguration.InputTrigger = TIM_TS_ITR0; // would not work with TIM5

    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_PWM2;
    timerOutputCompareConfiguration.Pulse = PWM_CLOCK_PERIOD; // start with passive output
    timerOutputCompareConfiguration.OCPolarity = TIM_OCPOLARITY_HIGH;
    timerOutputCompareConfiguration.OCFastMode = TIM_OCFAST_DISABLE;

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    // Red PWM output configuration
    pwmTimerRed.Instance = PWM_TIMER_RED;
    pwmTimerRed.Init.Prescaler = 0;
    pwmTimerRed.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerRed.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerRed.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerRed );

    HAL_TIM_ConfigClockSource( &pwmTimerRed, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerRed );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerRed, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    PWM_TIMER_RED->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Green PWM output configuration
    pwmTimerGreen.Instance = PWM_TIMER_GREEN;
    pwmTimerGreen.Init.Prescaler = 0;
    pwmTimerGreen.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerGreen.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerGreen.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerGreen );

    HAL_TIM_ConfigClockSource( &pwmTimerGreen, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerGreen );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerGreen, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    PWM_TIMER_GREEN->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Blue PWM output configuration
    pwmTimerBlue.Instance = PWM_TIMER_BLUE;
    pwmTimerBlue.Init.Prescaler = 0;
    pwmTimerBlue.Init.CounterMode = TIM_COUNTERMODE_UP;
    pwmTimerBlue.Init.Period = PWM_CLOCK_PERIOD - 1;
    pwmTimerBlue.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &pwmTimerBlue );

    HAL_TIM_ConfigClockSource( &pwmTimerBlue, &timerClockSourceConfiguration );

    HAL_TIM_PWM_Init( &pwmTimerBlue );

    HAL_TIM_SlaveConfigSynchronization( &pwmTimerBlue, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    PWM_TIMER_BLUE->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Timer GPIO configuration
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Pull = GPIO_PULLDOWN;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    gpioConfiguration.Pin = PWM_RED1_Pin|PWM_RED2_Pin|PWM_RED3_Pin|PWM_RED4_Pin;
    gpioConfiguration.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init( PWM_RED_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE1_Pin|PWM_BLUE2_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE1_2_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE3_Pin|PWM_BLUE4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE3_4_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_GREEN1_Pin|PWM_GREEN2_Pin|PWM_GREEN3_Pin|PWM_GREEN4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init( PWM_GREEN_GPIO_PORT, &gpioConfiguration );
}

void GridControl::startTimers()
{
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerRed.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerRed);

    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerGreen.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerGreen);

    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(pwmTimerBlue.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    __HAL_TIM_ENABLE(&pwmTimerBlue);

    TIM_CCxChannelCmd(baseInterruptTimer.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(baseInterruptTimer.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(baseInterruptTimer.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE);
    TIM_CCxChannelCmd(baseInterruptTimer.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
    HAL_TIM_Base_Start(&baseInterruptTimer);
}

extern "C" void DMA2_Stream5_IRQHandler()
{
    HAL_DMA_IRQHandler(&buttonInputDmaConfiguration);
}

} // namespace
