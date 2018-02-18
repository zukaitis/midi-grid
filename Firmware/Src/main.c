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
#include "usb/usb_device.h"
#include "usb/queue32.h"
#include "lcd/lcd.h"

#include "grid_buttons/grid_buttons.h"

//#define USE_SEMIHOSTING

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart6;

struct MidiMessage
{
    uint8_t header;
    uint8_t data[3];
};

union MidiInput
{
    uint32_t input;
    struct MidiMessage message;
};

union MidiInput midiInput;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint16_t brightnessTestResult[46000];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_ADC1_Init(void);

void printMidiMessage(union MidiInput message);
void randomLightAnimation();
void runBrigthnessTest();

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                
extern stB4Arrq rxq;

uint8_t sessionLayout[10][8] = {
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} };

uint8_t drumLayout[10][8] = {
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t topRowControllerNumbers[8] = {4, 7, 5, 6, 3, 2, 0, 1};

enum Layout
{
    Layout_SESSION = 0,
    Layout_USER1,
    Layout_USER2,
    Layout_RESERVED,
    Layout_VOLUME,
    Layout_PAN
};

static uint8_t currentLayout = Layout_SESSION;
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
    volatile uint32_t i=0;
    uint8_t buttonX, buttonY, event, velocity;
    uint8_t ledPositionX, ledPositionY;
    uint8_t codeIndexNumber, channel;
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  //MX_DMA_Init();
  //MX_SPI2_Init();
  //MX_TIM1_Init();
 // MX_USART6_UART_Init();
  MX_USB_DEVICE_Init();
 // MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
#ifdef USE_SEMIHOSTING
  initialise_monitor_handles(); // enable semihosting
#endif

  grid_initialize();
  grid_enable();

  LCD_init();
  LCD_print("yo", 12, 2);
  //runBrigthnessTest();
#ifdef USE_SEMIHOSTING
  printf("Semihosting output enabled\n");
#endif

  while (0 == rxq.num)
  {
      //randomLightAnimation();
      if (grid_getButtonEvent(&buttonX, &buttonY, &event))
      {
          if (isUsbConnected())
          {
              velocity = (event) ? 127 : 0;
              sendNoteOn(0,sessionLayout[buttonX][buttonY],velocity);
              processMidiMessage();
          }
          break;
      }
  }

  while (!isUsbConnected())
  {
    i++;
  }
#ifdef USE_SEMIHOSTING
  printf("Printing unacknowledged MIDI messages:\n");
#endif

  while (1)
  {
      i++;
      // led flash message - 0x15519109 (Hex)

        if (0 != rxq.num)
        {
            midiInput.input = *b4arrq_pop(&rxq);
            codeIndexNumber = midiInput.message.header & 0x0F;
            if ((0x9 == codeIndexNumber) || (0x8 == codeIndexNumber)) // note on or note off
            {
                if (Layout_USER1 == currentLayout)
                {
                    // only this layout uses drum layout
                    if ((midiInput.message.data[1] >= 36) && (midiInput.message.data[1] <= 107))
                    {
                        if (midiInput.message.data[1] <= 67)
                        {
                            ledPositionX = midiInput.message.data[1] % 4;
                            ledPositionY = (midiInput.message.data[1] - 36) / 4;
                        }
                        else if (midiInput.message.data[1] <= 99)
                        {
                            ledPositionX = midiInput.message.data[1] % 4 + 4;
                            ledPositionY = (midiInput.message.data[1] - 68) / 4;
                        }
                        else
                        {
                            ledPositionX = 8;
                            ledPositionY = 107 - midiInput.message.data[1];
                        }
                        channel = midiInput.message.data[0] & 0x0F;
                        if (channel > 2)
                        {
                            channel = 0;
                        }
                        if (0x8 == codeIndexNumber) // note off
                        {
                            grid_setLedFromMidiMessage(ledPositionX, ledPositionY, 0, 0);
                        }
                        else
                        {
                            grid_setLedFromMidiMessage(ledPositionX, ledPositionY, midiInput.message.data[2], channel);
                        }
                    }
                    else
                    {
                        printMidiMessage(midiInput);
                    }
                }
                else
                {
                    // not sure if this conditional is needed
                    if ((midiInput.message.data[1] >= 11) && (midiInput.message.data[1] <= 89))
                    {

                        ledPositionX = (midiInput.message.data[1] % 10) - 1;
                        ledPositionY = (midiInput.message.data[1] / 10) - 1;
                        channel = midiInput.message.data[0] & 0x0F;
                        if (channel > 2)
                        {
                            channel = 0;
                        }
                        grid_setLedFromMidiMessage(ledPositionX, ledPositionY, midiInput.message.data[2], channel);
                    }
                    else
                    {
                        printMidiMessage(midiInput);
                    }
                }
            }
            else if (0xB == codeIndexNumber) // change control
            {
                if ((midiInput.message.data[1] >= 104) && (midiInput.message.data[1] <= 111))
                {
                    ledPositionX = 9;
                    ledPositionY = topRowControllerNumbers[midiInput.message.data[1] - 104];
                    grid_setLedFromMidiMessage(ledPositionX, ledPositionY, midiInput.message.data[2], 0);
                }
                else
                {
                    printMidiMessage(midiInput);
                }
            }
            else if (0x7 == codeIndexNumber) // end of SysEx message
            {
                if (0x22 == midiInput.message.data[0]) // layout change message
                {
                    if (midiInput.message.data[1] < 6)
                    {
                        currentLayout = midiInput.message.data[1];
                        struct Colour colour = {0, 0, 0};
                        switch (currentLayout)
                        {
                            case Layout_SESSION:
                                colour.Red = 64;
                                break;
                            case Layout_USER1:
                                colour.Green = 64;
                                break;
                            case Layout_USER2:
                                colour.Blue = 64;
                                break;
                            case Layout_RESERVED:
                                colour.Red = 64;
                                colour.Green = 64;
                                break;
                            case Layout_PAN:
                                colour.Red = 64;
                                colour.Blue = 64;
                                break;
                            case Layout_VOLUME:
                                colour.Green = 64;
                                colour.Blue = 64;
                                break;
                        }
                        grid_setLedColour(9, 4, &colour);
                    }
                    else
                    {
                        printMidiMessage(midiInput);
                    }
                }
                else
                {
                    printMidiMessage(midiInput);
                }
            }
            else
            {
                printMidiMessage(midiInput);
            }
        }

        if (grid_getButtonEvent(&buttonX, &buttonY, &event))
        {
            velocity = (event != 0) ? 127 : 0;
            if (9 == buttonX) // control row
            {
                sendCtlChange( 0,sessionLayout[buttonX][buttonY],velocity );
                processMidiMessage();
            }
            else
            {
                if (Layout_SESSION == currentLayout)
                {
                    sendNoteOn( 0,sessionLayout[buttonX][buttonY],velocity );
                    processMidiMessage();
                }
                else if (Layout_USER1 == currentLayout)
                {
                    sendNoteOn( 7, drumLayout[buttonX][buttonY],velocity ); // can select channel between 6, 7 and 8
                    processMidiMessage();
                }
                else if (Layout_USER2 == currentLayout)
                {
                    sendNoteOn( 15, sessionLayout[buttonX][buttonY],velocity ); // can select channel between 14, 15 and 16
                    processMidiMessage();
                }
                else //if ((Layout_VOLUME == currentLayout) || (Layout_PAN == currentLayout))
                {
                    sendNoteOn( 0,sessionLayout[buttonX][buttonY],velocity );
                    processMidiMessage();
                }
//                else
//                {
//                    //don't send anything
//                }
            }
            LCD_print("zdrw jums", 12, 2);
        }
        grid_updateLeds();
    }
      /* USER CODE END 3 */
}

void printMidiMessage(union MidiInput message)
{
#ifdef USE_SEMIHOSTING
    uint8_t channel;
    uint8_t codeIndexNumber = midiInput.message.header & 0x0F;
    if (0x09 == codeIndexNumber)
    {
        channel = midiInput.message.data[0] & 0x0F;
        printf("NO, ch:%i n:%i v:%i\n", channel, midiInput.message.data[1], midiInput.message.data[2]);
    }
    else if (0x0B == codeIndexNumber)
    {
        channel = midiInput.message.data[0] & 0x0F;
        printf("CC, ch:%i c:%i v:%i\n", channel, midiInput.message.data[1], midiInput.message.data[2]);
    }
    else if (0x04 == codeIndexNumber)
    {
        if ((0x2000F004 == midiInput.input) || (0x18022904 == midiInput.input))
        {
            // ignore SysEx header messages
        }
        else
        {
            printf("SE, d: %02Xh %02Xh %02Xh\n", midiInput.message.data[0], midiInput.message.data[1], midiInput.message.data[2]);
        }
    }
    else if (0x07 == codeIndexNumber)
    {
        printf("SEe, d: %02Xh %02Xh %02Xh\n", midiInput.message.data[0], midiInput.message.data[1], midiInput.message.data[2]);
    }
    else
    {
        printf("Unknown message, CIN: %Xh\n", codeIndexNumber);
    }
#endif
}

void randomLightAnimation()
{
    static uint32_t newLightTime = 0;
    static uint8_t ledsChanged = 0;
    uint8_t ledPositionX, ledPositionY;
    uint8_t fullyLitColour;
    struct Colour colour;
    if (HAL_GetTick() >= newLightTime)
    {
        ledPositionX = rand() % 8;
        ledPositionY = rand() % 8;
        fullyLitColour = rand() % 3;
        if (0 == fullyLitColour)
        {
            colour.Red = 64;
            colour.Green = rand() % 65;
            colour.Blue = rand() % 65;
        }
        else if (1 == fullyLitColour)
        {
            colour.Red = rand() % 65;
            colour.Green = 64;
            colour.Blue = rand() % 65;
        }
        else if (2 == fullyLitColour)
        {
            colour.Red = rand() % 65;
            colour.Green = rand() % 65;
            colour.Blue = 64;
        }
        grid_setLedColour(ledPositionX, ledPositionY, &colour);
        newLightTime = HAL_GetTick() + 500 + rand() % 1000;
        ledsChanged++;
        if (ledsChanged > 63)
        {
            grid_setAllLedsOff();
            ledsChanged = 0;
        }
    }
}

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_RESET_Pin|LCD_DC_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : MIDI_OUT_DETECT_Pin */
  GPIO_InitStruct.Pin = MIDI_OUT_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RESET_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin|LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
