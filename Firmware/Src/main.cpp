/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "lcd/Lcd.h"

extern "C" {
#include "usb/usb_device.h"
#include "usb/queue32.h"
}

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

//SPI_HandleTypeDef hspi2;
//DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

//extern stB4Arrq rxq;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_ADC1_Init(void);

//uint16_t brightnessTestResult[46000];
//void runBrigthnessTest();

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
extern void initialise_monitor_handles(void); // semihosting
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
    ApplicationMain& applicationMain = ApplicationMain::getInstance();
    applicationMain.initialize();
    applicationMain.run(); // doesn't return
}

ApplicationMain::ApplicationMain() :
        grid( grid::Grid() ),
        switches( switches::Switches() ),
        gui( gui::Gui() ),
        usbMidi( midi::UsbMidi() ),
        launchpad( launchpad::Launchpad(grid, switches, gui, usbMidi) ),
        lcd_(lcd::Lcd::getInstance())
{}

ApplicationMain::~ApplicationMain()
{}

void ApplicationMain::initialize()
{
    HAL_Init();
    SystemClock_Config();

    //MX_GPIO_Init();
    // MX_USART6_UART_Init();
    MX_USB_DEVICE_Init();
    // MX_ADC1_Init();

    #ifdef USE_SEMIHOSTING
    initialise_monitor_handles(); // enable semihosting
    #endif

    grid.initialize();
    grid.enable();

    lcd_.initialize();
    lcd_.setBacklightIntensity( 55 );
//    lcd_.print("abcdefgh", lcd::WIDTH/2, 0, lcd::Justification_CENTER);
//    lcd_.print("ijklmnop", lcd::WIDTH/2, 9, lcd::Justification_CENTER);
//    lcd_.print("qrstuvwxyz", lcd::WIDTH/2, 18, lcd::Justification_CENTER);

    //runBrigthnessTest();
    #ifdef USE_SEMIHOSTING
    printf("Semihosting output enabled\n");
    #endif

}

void ApplicationMain::run()
{
    uint8_t buttonX, buttonY;
    ButtonEvent event;

    gui.displayConnectingImage();

    while (!isUsbConnected())
    {
        gui.refresh();
    }

    gui.displayWaitingForMidi();

    while (!usbMidi.isPacketAvailable())
    {
        randomLightAnimation();
        if (grid.getButtonEvent(&buttonX, &buttonY, &event))
        {
            break;
        }
        gui.refresh();
    }

    #ifdef USE_SEMIHOSTING
    printf("Printing unacknowledged MIDI messages:\n");
    #endif

    while (1)
    {
        launchpad.runProgram();
        // program only returns here when red button is pressed
        if (switches.isButtonPressed( 1 ))
        {
            runInternalMenu();
        }
    }
}

void ApplicationMain::runInternalMenu()
{
    uint8_t buttonX, buttonY;
    int8_t rotaryStep;
    ButtonEvent event;
    midi::MidiPacket unusedInputPacket;

    grid.discardAllPendingButtonEvents();
    switches.discardAllPendingEvents();

    gui.enterInternalMenu();

    grid.turnAllLedsOff();
    const Colour red = {64U, 0U, 0U};
    grid.setLed(7, 7, red);

    while (1)
    {
        usbMidi.getPacket(unusedInputPacket); // check for incoming packets and discard them

        if (grid.getButtonEvent( &buttonX, &buttonY, &event ))
        {
            if ((7 == buttonX) && (7 == buttonY))
            {
                // reset into DFU bootloader
                resetIntoBootloader();
            }
        }

        if (switches.getButtonEvent(&buttonX,  &event))
        {
            if ((1 == buttonX) && (!event))
            {
                break; // break internal menu loop, get back to launchpad mode
            }
        }

        switches.getRotaryEncoderEvent(&buttonX, &rotaryStep); // unused atm

        //grid.refreshLeds();
        gui.refresh();
    }
}

void ApplicationMain::resetIntoBootloader()
{
    // write these bytes into the end of RAM, so processor would jump into bootloader after reset
    // (there is condition in system_stm32f4xx.c that checks for this value at the beginning of a program)
    *((unsigned long *)0x2001FFF0) = 0xDEADBEEF;
    // Reset the processor
    NVIC_SystemReset();
}

void ApplicationMain::randomLightAnimation()
{
    static uint32_t newLightTime = 0;
    static uint8_t ledsChanged = 0;
    uint8_t ledPositionX, ledPositionY;
    uint8_t fullyLitColour;
    Colour colour;
    if (HAL_GetTick() >= newLightTime)
    {
        ledPositionX = rand() % 8;
        ledPositionY = rand() % 8;
        fullyLitColour = rand() % 3;
        switch (fullyLitColour)
        {
            case 0:
                colour.Red = 64;
                colour.Green = rand() % 65;
                colour.Blue = rand() % 65;
                break;
            case 1:
                colour.Red = rand() % 65;
                colour.Green = 64;
                colour.Blue = rand() % 65;
                break;
            case 2:
                colour.Red = rand() % 65;
                colour.Green = rand() % 65;
                colour.Blue = 64;
                break;
            default:
                break;
        }

        grid.setLed(ledPositionX, ledPositionY, colour);
        newLightTime = HAL_GetTick() + 500 + rand() % 1000;
        ledsChanged++;
        if (ledsChanged > 63)
        {
            grid.turnAllLedsOff();
            ledsChanged = 0;
        }
    }
}

#if 0
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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */

    __GPIOC_CLK_ENABLE();
    __ADC1_CLK_ENABLE();

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

/* SPI2 init function */
static void MX_SPI2_Init(void)
{
#if 0
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
#endif
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 15;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 49000;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

    HAL_TIM_MspPostInit(&htim1);
}

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{
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

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
#if 0
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
#endif

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void) // all but lcd and midi detect pin configurations copied
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pins : MIDI_OUT_DETECT_Pin */
    //GPIO_InitStruct.Pin = MIDI_OUT_DETECT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
