#include "grid/GridControl.h"

namespace grid
{
namespace grid_control
{

uint8_t GridControl::currentlyStableInputBufferIndex_ = 1;
bool GridControl::gridInputUpdated_ = false;
bool GridControl::switchInputUpdated_ = false;

uint32_t GridControl::buttonInput_[NkNumberOfButtonDebouncingCycles][kNumberOfVerticalSegments];
uint32_t GridControl::pwmOutputRed_[kNumberOfVerticalSegments][kNumberOfHorizontalSegments];
uint32_t GridControl::pwmOutputGreen_[kNumberOfVerticalSegments][kNumberOfHorizontalSegments];
uint32_t GridControl::pwmOutputBlue_[kNumberOfVerticalSegments][kNumberOfHorizontalSegments];

TIM_HandleTypeDef GridControl::pwmTimerRed_;
TIM_HandleTypeDef GridControl::pwmTimerGreen_;
TIM_HandleTypeDef GridControl::pwmTimerBlue_;
TIM_HandleTypeDef GridControl::baseInterruptTimer_;
DMA_HandleTypeDef GridControl::pwmOutputRedDmaConfiguration_;
DMA_HandleTypeDef GridControl::pwmOutputGreenDmaConfiguration_;
DMA_HandleTypeDef GridControl::pwmOutputBlueDmaConfiguration_;
DMA_HandleTypeDef GridControl::columnSelectDmaConfiguration_;

DMA_HandleTypeDef GridControl::buttonInputDmaConfiguration;

extern "C" void DMA2_Stream5_IRQHandler()
{
    HAL_DMA_IRQHandler(&GridControl::buttonInputDmaConfiguration);
}

GridControl::GridControl()
{
    for (uint8_t segment = 0; segment < kNumberOfVerticalSegments; segment++)
    {
        buttonInput_[0][segment] = 0x0000;
        buttonInput_[1][segment] = 0x0000;
    }
    turnAllLedsOff();
}

GridControl::~GridControl()
{
}

bool GridControl::getButtonInput( const uint8_t button ) const
{
    return (0 != (kNonGridButtonMask[button] & buttonInput_[currentlyStableInputBufferIndex_][0]));
}

uint8_t GridControl::getGridButtonInput( const uint8_t column ) const
{
    return static_cast<uint8_t>(kGridButtonMask & buttonInput_[currentlyStableInputBufferIndex_][column]);
}

uint8_t GridControl::getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const
{
    const uint8_t index = timeStep * 2;
    return (kRotaryEncoderMask[encoder] & buttonInput_[currentlyStableInputBufferIndex_][index])>>kRotaryEncoderBitShift[encoder];
}

void GridControl::initialize()
{
    initializeGpio();
    initializePwmTimers();
    initializePwmGpio();
    initializeBaseTimer();
    initializeDma();
}

bool GridControl::isButtonInputStable( const uint8_t button ) const
{
    return (0 == (kNonGridButtonMask[button] & (buttonInput_[0][0] ^ buttonInput_[1][0])));
}

bool GridControl::isGridVerticalSegmentInputStable( const uint8_t segment ) const
{
    return (0 == (kGridButtonMask & (buttonInput_[0][segment] ^ buttonInput_[1][segment])));
}

bool GridControl::isGridInputUpdated() const
{
    return gridInputUpdated_;
}

bool GridControl::isSwitchInputUpdated() const
{
    return switchInputUpdated_;
}

void GridControl::resetGridInputUpdatedFlag()
{
    gridInputUpdated_ = false;
}

void GridControl::resetSwitchInputUpdatedFlag()
{
    switchInputUpdated_ = false;
}

void GridControl::setLedColour( uint8_t ledPositionX, const uint8_t ledPositionY, const bool directLed, const Colour colour )
{
    ledPositionX = (ledPositionX + kNumberOfVerticalSegments - kTimerFrameOffset) % kNumberOfVerticalSegments;

    if (directLed)
    {
        pwmOutputRed_[ledPositionX][ledPositionY] = BRIGHTNESS_DIRECT[colour.Red];
        pwmOutputGreen_[ledPositionX][ledPositionY] = BRIGHTNESS_DIRECT[colour.Green];
        pwmOutputBlue_[ledPositionX][ledPositionY] = BRIGHTNESS_DIRECT[colour.Blue];
    }
    else
    {
        pwmOutputRed_[ledPositionX][ledPositionY] = BRIGHTNESS_THROUGH_PAD[colour.Red];
        pwmOutputGreen_[ledPositionX][ledPositionY] = BRIGHTNESS_THROUGH_PAD[colour.Green];
        pwmOutputBlue_[ledPositionX][ledPositionY] = BRIGHTNESS_THROUGH_PAD[colour.Blue];
    }
}

void GridControl::start()
{
    TIM_CCxChannelCmd( pwmTimerRed_.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed_.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed_.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed_.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerRed_);

    TIM_CCxChannelCmd( pwmTimerGreen_.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen_.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen_.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen_.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerGreen_ );

    TIM_CCxChannelCmd( pwmTimerBlue_.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue_.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue_.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue_.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerBlue_ );

    TIM_CCxChannelCmd( baseInterruptTimer_.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer_.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer_.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer_.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    HAL_TIM_Base_Start( &baseInterruptTimer_ );
}

void GridControl::turnAllLedsOff()
{
    for (uint8_t x = 0; x < kNumberOfVerticalSegments; x++)
    {
        for (uint8_t y = 0; y < kNumberOfHorizontalSegments; y++)
        {
            pwmOutputRed_[x][y] = BRIGHTNESS_DIRECT[0];
            pwmOutputGreen_[x][y] = BRIGHTNESS_DIRECT[0];
            pwmOutputBlue_[x][y] = BRIGHTNESS_DIRECT[0];
        }
    }
}

void GridControl::initializeBaseTimer()
{
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_MasterConfigTypeDef timerMasterConfiguration;

    __HAL_RCC_TIM1_CLK_ENABLE();

    baseInterruptTimer_.Instance = BASE_INTERRUPT_TIMER;
    baseInterruptTimer_.Init.Prescaler = kBaseInterruptClockPrescaler - 1;
    baseInterruptTimer_.Init.CounterMode = TIM_COUNTERMODE_UP;
    baseInterruptTimer_.Init.Period = kBaseInterruptClockPeriod - 1;
    baseInterruptTimer_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &baseInterruptTimer_ );

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource( &baseInterruptTimer_, &timerClockSourceConfiguration );

    timerMasterConfiguration.MasterOutputTrigger = TIM_TRGO_UPDATE;
    timerMasterConfiguration.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization( &baseInterruptTimer_, &timerMasterConfiguration );

    TIM_OC_InitTypeDef timerOutputCompareConfiguration;
    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_ACTIVE;

    timerOutputCompareConfiguration.Pulse = 1; // update as early as possible
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer_, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 7) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer_, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 8) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer_, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 9) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer_, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer_, TIM_DMA_CC1 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer_, TIM_DMA_CC2 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer_, TIM_DMA_CC3 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer_, TIM_DMA_CC4 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer_, TIM_DMA_UPDATE );
}

void GridControl::initializeDma()
{
    DMA_InitTypeDef ledOutputDmaInitConfiguration;

    __HAL_RCC_DMA2_CLK_ENABLE();

    columnSelectDmaConfiguration_.Instance = DMA2_Stream1;
    columnSelectDmaConfiguration_.Init.Channel = DMA_CHANNEL_6;
    columnSelectDmaConfiguration_.Init.Direction = DMA_MEMORY_TO_PERIPH;
    columnSelectDmaConfiguration_.Init.PeriphInc = DMA_PINC_DISABLE;
    columnSelectDmaConfiguration_.Init.MemInc = DMA_MINC_ENABLE;
    columnSelectDmaConfiguration_.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    columnSelectDmaConfiguration_.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    columnSelectDmaConfiguration_.Init.Mode = DMA_CIRCULAR;
    columnSelectDmaConfiguration_.Init.Priority = DMA_PRIORITY_HIGH;
    columnSelectDmaConfiguration_.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    columnSelectDmaConfiguration_.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    columnSelectDmaConfiguration_.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init( &columnSelectDmaConfiguration_ );
    __HAL_LINKDMA( &baseInterruptTimer_, hdma[TIM_DMA_ID_CC1], columnSelectDmaConfiguration_ );
    HAL_DMA_Start( &columnSelectDmaConfiguration_,
            reinterpret_cast<uint32_t>(&kColumnSelectValue[0]),
            reinterpret_cast<uint32_t>(&verticalSegmentControlGpioPort->ODR),
            kNumberOfVerticalSegments );

    ledOutputDmaInitConfiguration.Channel = DMA_CHANNEL_6;
    ledOutputDmaInitConfiguration.Direction = DMA_MEMORY_TO_PERIPH;
    ledOutputDmaInitConfiguration.PeriphInc = DMA_PINC_DISABLE;
    ledOutputDmaInitConfiguration.MemInc = DMA_MINC_ENABLE;
    ledOutputDmaInitConfiguration.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    ledOutputDmaInitConfiguration.MemDataAlignment = DMA_MDATAALIGN_WORD;
    ledOutputDmaInitConfiguration.Mode = DMA_CIRCULAR;
    ledOutputDmaInitConfiguration.Priority = DMA_PRIORITY_HIGH;
    ledOutputDmaInitConfiguration.FIFOMode = DMA_FIFOMODE_ENABLE;
    ledOutputDmaInitConfiguration.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    ledOutputDmaInitConfiguration.PeriphBurst = DMA_PBURST_INC4;

    pwmOutputRedDmaConfiguration_.Instance = DMA2_Stream2;
    pwmOutputRedDmaConfiguration_.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputRedDmaConfiguration_ );
    __HAL_LINKDMA( &baseInterruptTimer_, hdma[TIM_DMA_ID_CC2], pwmOutputRedDmaConfiguration_ );
    HAL_DMA_Start( &pwmOutputRedDmaConfiguration_,
            reinterpret_cast<uint32_t>(&pwmOutputRed_[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_RED->DMAR),
            kNumberOfVerticalSegments * kNumberOfHorizontalSegments );

    pwmOutputGreenDmaConfiguration_.Instance = DMA2_Stream6;
    pwmOutputGreenDmaConfiguration_.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init(&pwmOutputGreenDmaConfiguration_);
    __HAL_LINKDMA( &baseInterruptTimer_, hdma[TIM_DMA_ID_CC3], pwmOutputGreenDmaConfiguration_ );
    HAL_DMA_Start( &pwmOutputGreenDmaConfiguration_,
            reinterpret_cast<uint32_t>(&pwmOutputGreen_[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_GREEN->DMAR),
            kNumberOfVerticalSegments * kNumberOfHorizontalSegments );

    pwmOutputBlueDmaConfiguration_.Instance = DMA2_Stream4;
    pwmOutputBlueDmaConfiguration_.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputBlueDmaConfiguration_ );
    __HAL_LINKDMA( &baseInterruptTimer_, hdma[TIM_DMA_ID_CC4], pwmOutputBlueDmaConfiguration_ );
    HAL_DMA_Start( &pwmOutputBlueDmaConfiguration_,
            reinterpret_cast<uint32_t>(&pwmOutputBlue_[0][0]),
            reinterpret_cast<uint32_t>(&PWM_TIMER_BLUE->DMAR),
            kNumberOfVerticalSegments * kNumberOfHorizontalSegments );

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
    buttonInputDmaConfiguration.XferCpltCallback = &inputReadoutToMemory0CompleteCallback; //&inputReadoutToMemory0CompleteCallbackWrapper;
    buttonInputDmaConfiguration.XferM1CpltCallback = &inputReadoutToMemory1CompleteCallback; //&inputReadoutToMemory1CompleteCallbackWrapper;
    buttonInputDmaConfiguration.XferErrorCallback = &dmaErrorCallback;
    HAL_DMA_Init( &buttonInputDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer_, hdma[TIM_DMA_ID_UPDATE], buttonInputDmaConfiguration );

    HAL_NVIC_SetPriority( DMA2_Stream5_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( DMA2_Stream5_IRQn );

    HAL_DMAEx_MultiBufferStart_IT( &buttonInputDmaConfiguration,
            reinterpret_cast<uint32_t>(&GRID_BUTTON_IN_GPIO_PORT->IDR),
            reinterpret_cast<uint32_t>(&buttonInput_[0][0]),
            reinterpret_cast<uint32_t>(&buttonInput_[1][0]),
            kNumberOfVerticalSegments );
}

void GridControl::initializeGpio()
{
      static GPIO_InitTypeDef gpioConfiguration;

      // GPIO Ports Clock Enable
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      // Configure GPIO pin Output Level
      HAL_GPIO_WritePin( verticalSegmentControlGpioPort,
              COLUMN_OUT1_Pin | COLUMN_OUT2_Pin | COLUMN_OUT3_Pin |COLUMN_OUT4_Pin |COLUMN_OUT5_Pin |
                      COLUMN_OUT6_Pin,
              GPIO_PIN_SET );

      gpioConfiguration.Pin = BUTTON_IN1_Pin | BUTTON_IN2_Pin | ROTARY1_A_Pin | ROTARY1_B_Pin | ROTARY2_A_Pin| ROTARY2_B_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLUP;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = GRID_BUTTON_IN1_Pin | GRID_BUTTON_IN2_Pin | GRID_BUTTON_IN3_Pin | GRID_BUTTON_IN4_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLDOWN;
      HAL_GPIO_Init( GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = COLUMN_OUT1_Pin | COLUMN_OUT2_Pin | COLUMN_OUT3_Pin | COLUMN_OUT4_Pin |
              COLUMN_OUT5_Pin | COLUMN_OUT6_Pin;
      gpioConfiguration.Mode = GPIO_MODE_OUTPUT_OD;
      gpioConfiguration.Pull = GPIO_NOPULL;
      gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      HAL_GPIO_Init( verticalSegmentControlGpioPort, &gpioConfiguration );
}

void GridControl::initializePwmGpio()
{
    // initialize GPIO
    // making this structure static magically fixes the issues, but it's probably not the source of these issues
    static GPIO_InitTypeDef gpioConfiguration;

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

    gpioConfiguration.Pin = PWM_GREEN1_Pin|PWM_GREEN2_Pin|PWM_GREEN3_Pin|PWM_GREEN4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init( PWM_GREEN_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE1_Pin|PWM_BLUE2_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE1_2_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = PWM_BLUE3_Pin|PWM_BLUE4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( PWM_BLUE3_4_GPIO_PORT, &gpioConfiguration );
}

void GridControl::initializePwmTimers()
{
    TIM_Base_InitTypeDef timerBaseInitConfiguration;
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_OC_InitTypeDef timerOutputCompareConfiguration;
    TIM_SlaveConfigTypeDef timerSlaveConfiguration;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Same configuration for all three channels
    timerSlaveConfiguration.SlaveMode = TIM_SLAVEMODE_RESET;
    timerSlaveConfiguration.InputTrigger = TIM_TS_ITR0; // would not work with TIM5

    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_PWM1;
    timerOutputCompareConfiguration.Pulse = BRIGHTNESS_DIRECT[0]; // start with passive output
    timerOutputCompareConfiguration.OCPolarity = TIM_OCPOLARITY_HIGH;
    timerOutputCompareConfiguration.OCFastMode = TIM_OCFAST_DISABLE;

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    timerBaseInitConfiguration.Prescaler = kPwmClockPrescaler - 1;
    timerBaseInitConfiguration.CounterMode = TIM_COUNTERMODE_DOWN;
    timerBaseInitConfiguration.Period = kPwmClockPeriod - 1;
    timerBaseInitConfiguration.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    // Red PWM output configuration
    pwmTimerRed_.Instance = PWM_TIMER_RED;
    pwmTimerRed_.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerRed_ );
    HAL_TIM_ConfigClockSource( &pwmTimerRed_, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerRed_ );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerRed_, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed_, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed_, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed_, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed_, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerRed_.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Green PWM output configuration
    pwmTimerGreen_.Instance = PWM_TIMER_GREEN;
    pwmTimerGreen_.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerGreen_ );
    HAL_TIM_ConfigClockSource( &pwmTimerGreen_, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerGreen_ );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerGreen_, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen_, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen_, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen_, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen_, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerGreen_.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Blue PWM output configuration
    pwmTimerBlue_.Instance = PWM_TIMER_BLUE;
    pwmTimerBlue_.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerBlue_ );
    HAL_TIM_ConfigClockSource( &pwmTimerBlue_, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerBlue_ );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerBlue_, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue_, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue_, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue_, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue_, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerBlue_.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;
}

} // namespace grid_control
} // namespace grid
