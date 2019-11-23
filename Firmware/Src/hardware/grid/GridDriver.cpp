#include "hardware/grid/GridDriver.h"
#include "system/gpio_definitions.h"
#include "types/Coordinates.h"
#include "freertos/thread.hpp"

#include "stm32f4xx_hal.h"

namespace hardware
{
namespace grid
{

static const uint8_t kTimerFrameOffset = 1;

static const uint32_t kBaseInterruptClockPrescaler = 1;
static const uint32_t kBaseInterruptClockPeriod = 48000; // 500us
static const uint32_t kPwmClockPrescaler = 1;
static const uint16_t kPwmClockPeriod = 47000; // <500us - has to be shorter than base period

static TIM_TypeDef* const pwmTimerRedInstance = TIM2;
static TIM_TypeDef* const pwmTimerGreenInstance = TIM4;
static TIM_TypeDef* const pwmTimerBlueInstance = TIM3;
static TIM_TypeDef* const baseTimerInstance = TIM1;

static const etl::array<uint32_t, numberOfColumns> kColumnSelectValue = {
    0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
    0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
    0x78FF, 0x7AFF, 0xF8EF, 0xF9EF,
    0xFAEF, 0xFBEF, 0xFCEF, 0xFDEF,
    0xFEEF, 0xFFEF, 0x79FF, 0x7BFF };

static TIM_HandleTypeDef pwmTimerRed;
static TIM_HandleTypeDef pwmTimerGreen;
static TIM_HandleTypeDef pwmTimerBlue;
static TIM_HandleTypeDef baseInterruptTimer;
static DMA_HandleTypeDef buttonInputDmaConfiguration;
static DMA_HandleTypeDef pwmOutputRedDmaConfiguration;
static DMA_HandleTypeDef pwmOutputGreenDmaConfiguration;
static DMA_HandleTypeDef pwmOutputBlueDmaConfiguration;
static DMA_HandleTypeDef columnSelectDmaConfiguration;

extern "C" void DMA2_Stream5_IRQHandler()
{
    HAL_DMA_IRQHandler(&buttonInputDmaConfiguration);
}

static void inputReadoutToMemory0CompleteCallback( __DMA_HandleTypeDef* hdma )
{
    GridDriver::notifyInputReadoutToBuffer0Complete();
}

static void inputReadoutToMemory1CompleteCallback( __DMA_HandleTypeDef* hdma )
{
    GridDriver::notifyInputReadoutToBuffer1Complete();
}

static void dmaErrorCallback( __DMA_HandleTypeDef* hdma ) // unused
{
}

uint8_t GridDriver::stableBufferIndex_ = 0;

InputDebouncingBuffers GridDriver::input_ = {};
etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> GridDriver::redOutput_ = {};
etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> GridDriver::greenOutput_ = {};
etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> GridDriver::blueOutput_ = {};

etl::vector<freertos::Thread*, 7> GridDriver::threadToNotify_;

GridDriver::GridDriver()
{
}

void GridDriver::addThreadToNotify( freertos::Thread* const thread )
{
    threadToNotify_.push_back( thread );
}

const InputDebouncingBuffers& GridDriver::getInputDebouncingBuffers() const
{
    return input_;
}

const InputBuffer& GridDriver::getStableInputBuffer() const
{
    return input_[stableBufferIndex_];
}

void GridDriver::setRedOutput( const Coordinates& coords, const std::uint32_t value )
{
    redOutput_[coords.x][coords.y] = value;
}

void GridDriver::setGreenOutput( const Coordinates& coords, const std::uint32_t value )
{
    greenOutput_[coords.x][coords.y] = value;
}

void GridDriver::setBlueOutput( const Coordinates& coords, const std::uint32_t value )
{
    blueOutput_[coords.x][coords.y] = value;
}

void GridDriver::setAllOff()
{
    redOutput_ = {};
    greenOutput_ = {};
    blueOutput_ = {};
}

void GridDriver::initialize() const
{
    initializeGpio();
    initializePwmTimers();
    initializePwmGpio();
    initializeBaseTimer();
    initializeDma();
}

void GridDriver::start() const
{
    TIM_CCxChannelCmd( pwmTimerRed.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerRed.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerRed);

    TIM_CCxChannelCmd( pwmTimerGreen.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerGreen.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerGreen );

    TIM_CCxChannelCmd( pwmTimerBlue.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( pwmTimerBlue.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    __HAL_TIM_ENABLE( &pwmTimerBlue );

    TIM_CCxChannelCmd( baseInterruptTimer.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer.Instance, TIM_CHANNEL_3, TIM_CCx_ENABLE );
    TIM_CCxChannelCmd( baseInterruptTimer.Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE );
    HAL_TIM_Base_Start( &baseInterruptTimer );
}

void GridDriver::notifyInputReadoutToBuffer0Complete()
{
    stableBufferIndex_ = 0;
    notifyThreads();
}

void GridDriver::notifyInputReadoutToBuffer1Complete()
{
    stableBufferIndex_ = 1;
    notifyThreads();
}

void GridDriver::notifyThreads()
{
    for (freertos::Thread* thread : threadToNotify_)
    {
        thread->NotifyFromISR();
    }
}

void GridDriver::initializeBaseTimer() const
{
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_MasterConfigTypeDef timerMasterConfiguration;

    __HAL_RCC_TIM1_CLK_ENABLE();

    baseInterruptTimer.Instance = baseTimerInstance;
    baseInterruptTimer.Init.Prescaler = kBaseInterruptClockPrescaler - 1;
    baseInterruptTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    baseInterruptTimer.Init.Period = kBaseInterruptClockPeriod - 1;
    baseInterruptTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &baseInterruptTimer );

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource( &baseInterruptTimer, &timerClockSourceConfiguration );

    timerMasterConfiguration.MasterOutputTrigger = TIM_TRGO_UPDATE;
    timerMasterConfiguration.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization( &baseInterruptTimer, &timerMasterConfiguration );

    static TIM_OC_InitTypeDef timerOutputCompareConfiguration;
    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_ACTIVE;

    timerOutputCompareConfiguration.Pulse = 1; // update as early as possible
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 7) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 8) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );

    timerOutputCompareConfiguration.Pulse = (kBaseInterruptClockPeriod * 9) / 10;
    HAL_TIM_OC_ConfigChannel( &baseInterruptTimer, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );

    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC1 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC2 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC3 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_CC4 );
    __HAL_TIM_ENABLE_DMA( &baseInterruptTimer, TIM_DMA_UPDATE );
}

void GridDriver::initializeDma() const
{
    static DMA_InitTypeDef ledOutputDmaInitConfiguration;

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
            reinterpret_cast<uint32_t>(&kColumnSelectValue),
            reinterpret_cast<uint32_t>(&mcu::COLUMN_OUT_GPIO_PORT->ODR),
            kColumnSelectValue.size() );

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

    pwmOutputRedDmaConfiguration.Instance = DMA2_Stream2;
    pwmOutputRedDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputRedDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC2], pwmOutputRedDmaConfiguration );
    HAL_DMA_Start( &pwmOutputRedDmaConfiguration,
            reinterpret_cast<uint32_t>(&redOutput_),
            reinterpret_cast<uint32_t>(&pwmTimerRedInstance->DMAR),
            numberOfColumns * numberOfRows );

    pwmOutputGreenDmaConfiguration.Instance = DMA2_Stream6;
    pwmOutputGreenDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init(&pwmOutputGreenDmaConfiguration);
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC3], pwmOutputGreenDmaConfiguration );
    HAL_DMA_Start( &pwmOutputGreenDmaConfiguration,
            reinterpret_cast<uint32_t>(&greenOutput_),
            reinterpret_cast<uint32_t>(&pwmTimerGreenInstance->DMAR),
            numberOfColumns * numberOfRows );

    pwmOutputBlueDmaConfiguration.Instance = DMA2_Stream4;
    pwmOutputBlueDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputBlueDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC4], pwmOutputBlueDmaConfiguration );
    HAL_DMA_Start( &pwmOutputBlueDmaConfiguration,
            reinterpret_cast<uint32_t>(&blueOutput_),
            reinterpret_cast<uint32_t>(&pwmTimerBlueInstance->DMAR),
            numberOfColumns * numberOfRows );

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
    buttonInputDmaConfiguration.XferCpltCallback = &inputReadoutToMemory0CompleteCallback;
    buttonInputDmaConfiguration.XferM1CpltCallback = &inputReadoutToMemory1CompleteCallback;
    buttonInputDmaConfiguration.XferErrorCallback = &dmaErrorCallback;
    HAL_DMA_Init( &buttonInputDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_UPDATE], buttonInputDmaConfiguration );

    HAL_NVIC_SetPriority( DMA2_Stream5_IRQn, 6, 0 );
    HAL_NVIC_EnableIRQ( DMA2_Stream5_IRQn );

    HAL_DMAEx_MultiBufferStart_IT( &buttonInputDmaConfiguration,
            reinterpret_cast<uint32_t>(&mcu::GRID_BUTTON_IN_GPIO_PORT->IDR),
            reinterpret_cast<uint32_t>(&input_[0]),
            reinterpret_cast<uint32_t>(&input_[1]),
            numberOfColumns );
}

void GridDriver::initializeGpio() const
{
      static GPIO_InitTypeDef gpioConfiguration;

      // GPIO Ports Clock Enable
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      // Configure GPIO pin Output Level
      HAL_GPIO_WritePin( mcu::COLUMN_OUT_GPIO_PORT,
              mcu::COLUMN_OUT1_Pin | mcu::COLUMN_OUT2_Pin | mcu::COLUMN_OUT3_Pin | mcu::COLUMN_OUT4_Pin |
              mcu::COLUMN_OUT5_Pin | mcu::COLUMN_OUT6_Pin,
              GPIO_PIN_SET );

      gpioConfiguration.Pin = mcu::BUTTON_IN1_Pin | mcu::BUTTON_IN2_Pin | mcu::ROTARY1_A_Pin | mcu::ROTARY1_B_Pin |
              mcu::ROTARY2_A_Pin| mcu::ROTARY2_B_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLUP;
      HAL_GPIO_Init( mcu::GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = mcu::GRID_BUTTON_IN1_Pin | mcu::GRID_BUTTON_IN2_Pin | mcu::GRID_BUTTON_IN3_Pin |
              mcu::GRID_BUTTON_IN4_Pin;
      gpioConfiguration.Mode = GPIO_MODE_INPUT;
      gpioConfiguration.Pull = GPIO_PULLDOWN;
      HAL_GPIO_Init( mcu::GRID_BUTTON_IN_GPIO_PORT, &gpioConfiguration );

      gpioConfiguration.Pin = mcu::COLUMN_OUT1_Pin | mcu::COLUMN_OUT2_Pin | mcu::COLUMN_OUT3_Pin | mcu::COLUMN_OUT4_Pin |
              mcu::COLUMN_OUT5_Pin | mcu::COLUMN_OUT6_Pin;
      gpioConfiguration.Mode = GPIO_MODE_OUTPUT_OD;
      gpioConfiguration.Pull = GPIO_NOPULL;
      gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      HAL_GPIO_Init( mcu::COLUMN_OUT_GPIO_PORT, &gpioConfiguration );
}

void GridDriver::initializePwmGpio() const
{
    // initialize GPIO
    static GPIO_InitTypeDef gpioConfiguration;

    // Timer GPIO configuration
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpioConfiguration.Mode = GPIO_MODE_AF_PP;
    gpioConfiguration.Pull = GPIO_PULLDOWN;
    gpioConfiguration.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    gpioConfiguration.Pin = mcu::PWM_RED1_Pin | mcu::PWM_RED2_Pin | mcu::PWM_RED3_Pin | mcu::PWM_RED4_Pin;
    gpioConfiguration.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init( mcu::PWM_RED_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = mcu::PWM_GREEN1_Pin | mcu::PWM_GREEN2_Pin | mcu::PWM_GREEN3_Pin | mcu::PWM_GREEN4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init( mcu::PWM_GREEN_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = mcu::PWM_BLUE1_Pin | mcu::PWM_BLUE2_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( mcu::PWM_BLUE1_2_GPIO_PORT, &gpioConfiguration );

    gpioConfiguration.Pin = mcu::PWM_BLUE3_Pin | mcu::PWM_BLUE4_Pin;
    gpioConfiguration.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init( mcu::PWM_BLUE3_4_GPIO_PORT, &gpioConfiguration );
}

void GridDriver::initializePwmTimers() const
{
    static TIM_Base_InitTypeDef timerBaseInitConfiguration;
    static TIM_OC_InitTypeDef timerOutputCompareConfiguration;
    TIM_ClockConfigTypeDef timerClockSourceConfiguration;
    TIM_SlaveConfigTypeDef timerSlaveConfiguration;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Same configuration for all three channels
    timerSlaveConfiguration.SlaveMode = TIM_SLAVEMODE_RESET;
    timerSlaveConfiguration.InputTrigger = TIM_TS_ITR0; // would not work with TIM5

    timerOutputCompareConfiguration.OCMode = TIM_OCMODE_PWM1;
    timerOutputCompareConfiguration.Pulse = 0; // start with passive output
    timerOutputCompareConfiguration.OCPolarity = TIM_OCPOLARITY_HIGH;
    timerOutputCompareConfiguration.OCFastMode = TIM_OCFAST_DISABLE;

    timerClockSourceConfiguration.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    timerBaseInitConfiguration.Prescaler = kPwmClockPrescaler - 1;
    timerBaseInitConfiguration.CounterMode = TIM_COUNTERMODE_DOWN;
    timerBaseInitConfiguration.Period = kPwmClockPeriod - 1;
    timerBaseInitConfiguration.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    // Red PWM output configuration
    pwmTimerRed.Instance = pwmTimerRedInstance;
    pwmTimerRed.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerRed );
    HAL_TIM_ConfigClockSource( &pwmTimerRed, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerRed );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerRed, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerRed, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerRed.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Green PWM output configuration
    pwmTimerGreen.Instance = pwmTimerGreenInstance;
    pwmTimerGreen.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerGreen );
    HAL_TIM_ConfigClockSource( &pwmTimerGreen, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerGreen );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerGreen, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerGreen, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerGreen.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;

    // Blue PWM output configuration
    pwmTimerBlue.Instance = pwmTimerBlueInstance;
    pwmTimerBlue.Init = timerBaseInitConfiguration;
    HAL_TIM_Base_Init( &pwmTimerBlue );
    HAL_TIM_ConfigClockSource( &pwmTimerBlue, &timerClockSourceConfiguration );
    HAL_TIM_PWM_Init( &pwmTimerBlue );
    HAL_TIM_SlaveConfigSynchronization( &pwmTimerBlue, &timerSlaveConfiguration );

    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_1 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_2 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_3 );
    HAL_TIM_PWM_ConfigChannel( &pwmTimerBlue, &timerOutputCompareConfiguration, TIM_CHANNEL_4 );
    // set up timer's DMA input register (DMAR) to pass data into 4 registers starting with CCR1
    pwmTimerBlue.Instance->DCR = TIM_DMABURSTLENGTH_4TRANSFERS | TIM_DMABASE_CCR1;
}

} // namespace grid
} // namespace hardware
