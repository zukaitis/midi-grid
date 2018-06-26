#ifndef GRID_BUTTONS_GRIDCONTROL_H_
#define GRID_BUTTONS_GRIDCONTROL_H_

#include "Types.h"
#include "stm32f4xx_hal.h"

namespace grid
{
namespace grid_control
{

static const uint8_t NUMBER_OF_ROWS = 4;
static const uint8_t NUMBER_OF_COLUMNS = 20;
static const uint8_t NUMBER_OF_BUTTON_DEBOUNCING_CYCLES = 2;
static const uint8_t TIMER_FRAME_OFFSET = 1;

static const uint32_t PWM_CLOCK_PRESCALER = 1; // 96; // 1us
static const uint16_t PWM_CLOCK_PERIOD = 47000; // <500us - has to be shorter than base period
static const uint32_t BASE_INTERRUPT_CLOCK_PRESCALER = 1; // 96; // 1us
static const uint32_t BASE_INTERRUPT_CLOCK_PERIOD = 48000; // 500; // 500us

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

static const uint16_t BRIGHTNESS_THROUGH_PAD[65] = {
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

static const uint16_t BRIGHTNESS_DIRECT[65] = {
        0, 223, 308, 397, 494, 598, 709, 825,
        947, 1075, 1208, 1348, 1491, 1639, 1793, 1950,
        2113, 2279, 2448, 2621, 2802, 2981, 3164, 3354,
        3542, 3738, 3931, 4133, 4337, 4542, 4748, 4952,
        5166, 5382, 5591, 5809, 6032, 6259, 6473, 6699,
        6922, 7155, 7385, 7607, 7856, 8079, 8319, 8551,
        8783, 9021, 9265, 9500, 9753, 9995, 10233, 10489,
        10737, 10989, 11213, 11489, 11729, 11987, 12203, 12480, 12741 };

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

    void setLedColour( uint8_t ledPositionX, const uint8_t ledPositionY, const bool directLed, const Colour colour );
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

    void initPwmGpio();

    uint8_t currentlyStableInputBuffer_;

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

} // namespace grid_control
} // namespace grid
#endif // GRID_BUTTONS_GRIDCONTROL_H_
