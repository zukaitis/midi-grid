#ifndef GRID_BUTTONS_GRIDCONTROL_H_
#define GRID_BUTTONS_GRIDCONTROL_H_

#include "stm32f4xx_hal.h"
#include "Types.h"

namespace grid_control
{

static const uint8_t NUMBER_OF_ROWS = 4;
static const uint8_t NUMBER_OF_COLUMNS = 20;
static const uint8_t NUMBER_OF_BUTTON_DEBOUNCING_CYCLES = 2;
static const uint8_t TIMER_FRAME_OFFSET = 1;

static const uint16_t PWM_CLOCK_PERIOD = 47000; // <500us - has to be shorter than base period
static const uint32_t BASE_INTERRUPT_CLOCK_PRESCALER = 96; // 1us
static const uint32_t BASE_INTERRUPT_CLOCK_PERIOD = 500; // 500us

static const uint16_t GRID_BUTTON_MASK = 0x000F;
static const uint16_t BUTTON_MASK[2] = {0x2000, 0x0400};
static const uint16_t ROTARY_ENCODER_MASK[2] = {0xC000, 0x1800};
static const uint16_t ROTARY_ENCODER_SHIFT[2] = {14, 11};

static GPIO_TypeDef* const GRID_BUTTON_IN_GPIO_PORT = GPIOC;
static const uint16_t BUTTON_IN1_Pin = GPIO_PIN_13;
static const uint16_t BUTTON_IN2_Pin = GPIO_PIN_10;
static const uint16_t ROTARY1_A_Pin = GPIO_PIN_14;
static const uint16_t ROTARY1_B_Pin = GPIO_PIN_15;
static const uint16_t ROTARY2_A_Pin = GPIO_PIN_11;
static const uint16_t ROTARY2_B_Pin = GPIO_PIN_12;
static const uint16_t GRID_BUTTON_IN1_Pin = GPIO_PIN_0;
static const uint16_t GRID_BUTTON_IN2_Pin = GPIO_PIN_1;
static const uint16_t GRID_BUTTON_IN3_Pin = GPIO_PIN_2;
static const uint16_t GRID_BUTTON_IN4_Pin = GPIO_PIN_3;

static GPIO_TypeDef* const GRID_COLUMN_CONTROL_GPIO_PORT = GPIOA;
static const uint16_t GRID_COLUMN_OUT1_Pin = GPIO_PIN_8;
static const uint16_t GRID_COLUMN_OUT2_Pin = GPIO_PIN_9;
static const uint16_t GRID_COLUMN_OUT3_Pin = GPIO_PIN_10;
static const uint16_t GRID_COLUMN_OUT4_Pin = GPIO_PIN_5;
static const uint16_t GRID_COLUMN_OUT5_Pin = GPIO_PIN_4;
static const uint16_t GRID_COLUMN_OUT6_Pin = GPIO_PIN_15;

static GPIO_TypeDef* const PWM_RED_GPIO_PORT = GPIOA;
static const uint16_t PWM_RED1_Pin = GPIO_PIN_0;
static const uint16_t PWM_RED2_Pin = GPIO_PIN_1;
static const uint16_t PWM_RED3_Pin = GPIO_PIN_2;
static const uint16_t PWM_RED4_Pin = GPIO_PIN_3;

static GPIO_TypeDef* const PWM_GREEN_GPIO_PORT = GPIOB;
const uint16_t PWM_GREEN1_Pin = GPIO_PIN_6;
const uint16_t PWM_GREEN2_Pin = GPIO_PIN_7;
const uint16_t PWM_GREEN3_Pin = GPIO_PIN_8;
const uint16_t PWM_GREEN4_Pin = GPIO_PIN_9;

static GPIO_TypeDef* const PWM_BLUE1_2_GPIO_PORT = GPIOA;
static const uint16_t PWM_BLUE1_Pin = GPIO_PIN_6;
static const uint16_t PWM_BLUE2_Pin = GPIO_PIN_7;
static GPIO_TypeDef* const PWM_BLUE3_4_GPIO_PORT = GPIOB;
static const uint16_t PWM_BLUE3_Pin = GPIO_PIN_0;
static const uint16_t PWM_BLUE4_Pin = GPIO_PIN_1;

static TIM_TypeDef* const PWM_TIMER_RED = TIM2;
static TIM_TypeDef* const PWM_TIMER_GREEN = TIM4;
static TIM_TypeDef* const PWM_TIMER_BLUE = TIM3;
static TIM_TypeDef* const BASE_INTERRUPT_TIMER = TIM1;

static const uint16_t brightnessPad[65] = {
        47000, 46662, 46365, 46057, 45727, 45373, 44999, 44607,
        44195, 43763, 43317, 42851, 42373, 41879, 41359, 40843,
        40287, 39741, 39177, 38600, 37993, 37397, 36771, 36144,
        35499, 34851, 34185, 33501, 32796, 32108, 31429, 30721,
        29973, 29231, 28494, 27733, 26952, 26156, 25383, 24564,
        23781, 22934, 22103, 21275, 20401, 19570, 18699, 17802,
        16959, 16053, 15165, 14210, 13317, 12399, 11433, 10421,
        9485, 8522, 7585, 6597, 5642, 4627, 3641, 2591, 1001 };

static const uint16_t brightnessDirect[65] = {
        47000, 46777, 46692, 46603, 46506, 46402, 46291, 46175,
        46053, 45925, 45792, 45652, 45509, 45361, 45207, 45050,
        44887, 44721, 44552, 44379, 44198, 44019, 43836, 43646,
        43458, 43262, 43069, 42867, 42663, 42458, 42252, 42048,
        41834, 41618, 41409, 41191, 40968, 40741, 40527, 40301,
        40078, 39845, 39615, 39393, 39144, 38921, 38681, 38449,
        38217, 37979, 37735, 37500, 37247, 37005, 36767, 36511,
        36263, 36011, 35787, 35511, 35271, 35013, 34797, 34520, 34259 };

static const uint32_t columnSelectValue[NUMBER_OF_COLUMNS] = {
        0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
        0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
        0x78FF, 0x7AFF, 0xF8EF, 0xF9EF,
        0xFAEF, 0xFBEF, 0xFCEF, 0xFDEF,
        0xFEEF, 0xFFEF, 0x79FF, 0x7BFF };

class GridControl
{
public:

    // singleton, because class uses an interrupt
    static inline GridControl& getInstance()
    {
        static GridControl instance;
        return instance;
    }

    ~GridControl();

    bool getButtonInput( const uint8_t button ) const;
    uint8_t getGridButtonInput( const uint8_t column ) const;
    uint8_t getRotaryEncodersInput( const uint8_t encoder, uint8_t step ) const;
    void initialize();

    inline void inputReadoutToMemory0CompleteCallback()
    {
        currentlyStableInputBuffer_ = 0;
        gridInputUpdated = true;
        switchInputUpdated = true;
    }

    inline void inputReadoutToMemory1CompleteCallback()
    {
        currentlyStableInputBuffer_ = 1;
        gridInputUpdated = true;
        switchInputUpdated = true;
    }

    bool isButtonInputStable( const uint8_t button ) const;
    bool isGridColumnInputStable( const uint8_t column ) const;

    void setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, bool directLed, const Colour colour );
    void startTimers();
    void turnAllLedsOff();

    bool gridInputUpdated = false;
    bool switchInputUpdated = false;

private:
    GridControl();

    void initializeBaseTimer();
    void initializeDma();
    void initializeGpio();
    void initializePwmOutputs();

    uint8_t currentlyStableInputBuffer_ = 0;

    uint32_t pwmOutputRed_[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    uint32_t pwmOutputGreen_[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    uint32_t pwmOutputBlue_[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    uint32_t buttonInput_[NUMBER_OF_BUTTON_DEBOUNCING_CYCLES][NUMBER_OF_COLUMNS];

    TIM_HandleTypeDef pwmTimerRed_;
    TIM_HandleTypeDef pwmTimerGreen_;
    TIM_HandleTypeDef pwmTimerBlue_;
    TIM_HandleTypeDef baseInterruptTimer_;
    DMA_HandleTypeDef pwmOutputRedDmaConfiguration_;
    DMA_HandleTypeDef pwmOutputGreenDmaConfiguration_;
    DMA_HandleTypeDef pwmOutputBlueDmaConfiguration_;
    DMA_HandleTypeDef columnSelectDmaConfiguration_;
};

} // namespace
#endif /* GRID_BUTTONS_GRIDCONTROL_H_ */
