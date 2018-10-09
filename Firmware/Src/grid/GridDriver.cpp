#include "grid/GridDriver.h"
#include "system/gpio_definitions.h"

#include "stm32f4xx_hal.h"

namespace grid
{

static const uint8_t kTimerFrameOffset = 1;

static const uint32_t kBaseInterruptClockPrescaler = 1;
static const uint32_t kBaseInterruptClockPeriod = 48000; // 500us
static const uint32_t kPwmClockPrescaler = 1;
static const uint16_t kPwmClockPeriod = 47000; // <500us - has to be shorter than base period

static const uint16_t kGridButtonMask = 0x000F;
static const uint16_t kNonGridButtonMask[2] = {0x2000, 0x0400};
static const uint16_t kRotaryEncoderMask[2] = {0xC000, 0x1800};
static const uint16_t kRotaryEncoderBitShift[2] = {14, 11};

static const uint8_t kNumberOfLedColourIntensityLevels = 65;

static TIM_TypeDef* const pwmTimerRedInstance = TIM2;
static TIM_TypeDef* const pwmTimerGreenInstance = TIM4;
static TIM_TypeDef* const pwmTimerBlueInstance = TIM3;
static TIM_TypeDef* const baseTimerInstance = TIM1;

static const uint16_t kBrightnessThroughPad[kNumberOfLedColourIntensityLevels] = {
        0, 338, 635, 943, 1273, 1627, 2001, 2393,
        2805, 3237, 3683, 4149, 4627, 5121, 5641, 6157,
        6713, 7259, 7823, 8400, 9007, 9603, 10229, 10856,
        11501, 12149, 12815, 13499, 14204, 14892, 15571, 16279,
        17027, 17769, 18506, 19267, 20048, 20844, 21617, 22436,
        23219, 24066, 24897, 25725, 26599, 27430, 28301, 29198,
        30041, 30947, 31835, 32790, 33683, 34601, 35567, 36579,
        37515, 38478, 39415, 40403, 41358, 42373, 43359, 44409, 46000 };
//        0, 338, 341, 349, 363, 384, 414, 453,
//        503, 566, 641, 730, 834, 954, 1091, 1245,
//        1418, 1611, 1823, 2057, 2312, 2591, 2892, 3218,
//        3569, 3945, 4348, 4779, 5237, 5724, 6240, 6787,
//        7364, 7973, 8614, 9288, 9995, 10737, 11513, 12325,
//        13173, 14058, 14981, 15941, 16941, 17979, 19058, 20177,
//        21338, 22540, 23785, 25072, 26404, 27779, 29200, 30666,
//        32178, 33737, 35342, 36996, 38698, 40449, 42249, 44099, 46000 };

static const uint16_t kBrightnessDirect[kNumberOfLedColourIntensityLevels] = {
        0, 223, 308, 397, 494, 598, 709, 825,
        947, 1075, 1208, 1348, 1491, 1639, 1793, 1950,
        2113, 2279, 2448, 2621, 2802, 2981, 3164, 3354,
        3542, 3738, 3931, 4133, 4337, 4542, 4748, 4952,
        5166, 5382, 5591, 5809, 6032, 6259, 6473, 6699,
        6922, 7155, 7385, 7607, 7856, 8079, 8319, 8551,
        8783, 9021, 9265, 9500, 9753, 9995, 10233, 10489,
        10737, 10989, 11213, 11489, 11729, 11987, 12203, 12480, 12741 };

static const uint32_t kColumnSelectValue[GridDriver::numberOfVerticalSegments] = {
        0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
        0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
        0x78FF, 0x7AFF, 0xF8EF, 0xF9EF,
        0xFAEF, 0xFBEF, 0xFCEF, 0xFDEF,
        0xFEEF, 0xFFEF, 0x79FF, 0x7BFF };

uint8_t GridDriver::currentlyStableInputBufferIndex_ = 1;
bool GridDriver::gridInputUpdated_ = false;
bool GridDriver::switchInputUpdated_ = false;

uint32_t GridDriver::buttonInput_[nNumberOfButtonDebouncingCycles][numberOfVerticalSegments];
uint32_t GridDriver::pwmOutputRed_[numberOfVerticalSegments][numberOfHorizontalSegments];
uint32_t GridDriver::pwmOutputGreen_[numberOfVerticalSegments][numberOfHorizontalSegments];
uint32_t GridDriver::pwmOutputBlue_[numberOfVerticalSegments][numberOfHorizontalSegments];

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

static void inputReadoutToMemory0CompleteCallbackWrapper( __DMA_HandleTypeDef* hdma )
{
    GridDriver::inputReadoutToMemory0CompleteCallback();
}

static void inputReadoutToMemory1CompleteCallbackWrapper( __DMA_HandleTypeDef* hdma )
{
    GridDriver::inputReadoutToMemory1CompleteCallback();
}

static void dmaErrorCallback( __DMA_HandleTypeDef* hdma ) // unused
{
}

GridDriver::GridDriver()
{
    for (uint8_t segment = 0; segment < numberOfVerticalSegments; segment++)
    {
        buttonInput_[0][segment] = 0x0000;
        buttonInput_[1][segment] = 0x0000;
    }
    turnAllLedsOff();
}

GridDriver::~GridDriver()
{
}

bool GridDriver::getButtonInput( const uint8_t button ) const
{
    return (0 != (kNonGridButtonMask[button] & buttonInput_[currentlyStableInputBufferIndex_][0]));
}

uint8_t GridDriver::getGridButtonInput( const uint8_t column ) const
{
    return static_cast<uint8_t>(kGridButtonMask & buttonInput_[currentlyStableInputBufferIndex_][column]);
}

uint8_t GridDriver::getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const
{
    const uint8_t index = timeStep * 2;
    return (kRotaryEncoderMask[encoder] & buttonInput_[currentlyStableInputBufferIndex_][index])>>kRotaryEncoderBitShift[encoder];
}

void GridDriver::initialize()
{
    initializeGpio();
    initializePwmTimers();
    initializePwmGpio();
    initializeBaseTimer();
    initializeDma();
}

bool GridDriver::isButtonInputStable( const uint8_t button ) const
{
    return (0 == (kNonGridButtonMask[button] & (buttonInput_[0][0] ^ buttonInput_[1][0])));
}

bool GridDriver::isGridVerticalSegmentInputStable( const uint8_t segment ) const
{
    return (0 == (kGridButtonMask & (buttonInput_[0][segment] ^ buttonInput_[1][segment])));
}

bool GridDriver::isGridInputUpdated() const
{
    return gridInputUpdated_;
}

bool GridDriver::isSwitchInputUpdated() const
{
    return switchInputUpdated_;
}

void GridDriver::resetGridInputUpdatedFlag()
{
    gridInputUpdated_ = false;
}

void GridDriver::resetSwitchInputUpdatedFlag()
{
    switchInputUpdated_ = false;
}

void GridDriver::setLedColour( uint8_t ledPositionX, const uint8_t ledPositionY, const bool directLed, const Colour colour )
{
    ledPositionX = (ledPositionX + numberOfVerticalSegments - kTimerFrameOffset) % numberOfVerticalSegments;

    if (directLed)
    {
        pwmOutputRed_[ledPositionX][ledPositionY] = kBrightnessDirect[colour.Red];
        pwmOutputGreen_[ledPositionX][ledPositionY] = kBrightnessDirect[colour.Green];
        pwmOutputBlue_[ledPositionX][ledPositionY] = kBrightnessDirect[colour.Blue];
    }
    else
    {
        pwmOutputRed_[ledPositionX][ledPositionY] = kBrightnessThroughPad[colour.Red];
        pwmOutputGreen_[ledPositionX][ledPositionY] = kBrightnessThroughPad[colour.Green];
        pwmOutputBlue_[ledPositionX][ledPositionY] = kBrightnessThroughPad[colour.Blue];
    }
}

void GridDriver::start()
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

void GridDriver::turnAllLedsOff()
{
    for (uint8_t x = 0; x < numberOfVerticalSegments; x++)
    {
        for (uint8_t y = 0; y < numberOfHorizontalSegments; y++)
        {
            pwmOutputRed_[x][y] = kBrightnessDirect[0];
            pwmOutputGreen_[x][y] = kBrightnessDirect[0];
            pwmOutputBlue_[x][y] = kBrightnessDirect[0];
        }
    }
}

void GridDriver::initializeBaseTimer()
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

void GridDriver::initializeDma()
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
            reinterpret_cast<uint32_t>(&kColumnSelectValue[0]),
            reinterpret_cast<uint32_t>(&mcu::COLUMN_OUT_GPIO_PORT->ODR),
            numberOfVerticalSegments );

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
            reinterpret_cast<uint32_t>(&pwmOutputRed_[0][0]),
            reinterpret_cast<uint32_t>(&pwmTimerRedInstance->DMAR),
            numberOfVerticalSegments * numberOfHorizontalSegments );

    pwmOutputGreenDmaConfiguration.Instance = DMA2_Stream6;
    pwmOutputGreenDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init(&pwmOutputGreenDmaConfiguration);
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC3], pwmOutputGreenDmaConfiguration );
    HAL_DMA_Start( &pwmOutputGreenDmaConfiguration,
            reinterpret_cast<uint32_t>(&pwmOutputGreen_[0][0]),
            reinterpret_cast<uint32_t>(&pwmTimerGreenInstance->DMAR),
            numberOfVerticalSegments * numberOfHorizontalSegments );

    pwmOutputBlueDmaConfiguration.Instance = DMA2_Stream4;
    pwmOutputBlueDmaConfiguration.Init = ledOutputDmaInitConfiguration;
    HAL_DMA_Init( &pwmOutputBlueDmaConfiguration );
    __HAL_LINKDMA( &baseInterruptTimer, hdma[TIM_DMA_ID_CC4], pwmOutputBlueDmaConfiguration );
    HAL_DMA_Start( &pwmOutputBlueDmaConfiguration,
            reinterpret_cast<uint32_t>(&pwmOutputBlue_[0][0]),
            reinterpret_cast<uint32_t>(&pwmTimerBlueInstance->DMAR),
            numberOfVerticalSegments * numberOfHorizontalSegments );

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
            reinterpret_cast<uint32_t>(&mcu::GRID_BUTTON_IN_GPIO_PORT->IDR),
            reinterpret_cast<uint32_t>(&buttonInput_[0][0]),
            reinterpret_cast<uint32_t>(&buttonInput_[1][0]),
            numberOfVerticalSegments );
}

void GridDriver::initializeGpio()
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

void GridDriver::initializePwmGpio()
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

void GridDriver::initializePwmTimers()
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
    timerOutputCompareConfiguration.Pulse = kBrightnessDirect[0]; // start with passive output
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
