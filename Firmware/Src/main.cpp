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


#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles(void); // semihosting
#endif

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
    configureNvicPriorities();
    configureSystemClock();

    MX_USB_DEVICE_Init();

    #ifdef USE_SEMIHOSTING
    initialise_monitor_handles(); // enable semihosting
    #endif

    grid.initialize();
    grid.enable();

    lcd_.initialize();
    lcd_.setBacklightIntensity( 55 );

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
    grid.setLed(7, 0, red);

    while (1)
    {
        usbMidi.getPacket(unusedInputPacket); // check for incoming packets and discard them

        if (grid.getButtonEvent( &buttonX, &buttonY, &event ))
        {
            if ((7 == buttonX) && (0 == buttonY))
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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void ApplicationMain::configureSystemClock()
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    // Configure the main internal regulator output voltage
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

    // Configure the Systick interrupt time
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    // Configure the Systick
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    // SysTick_IRQn interrupt configuration
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void ApplicationMain::configureNvicPriorities()
{
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // System interrupt init
    // MemoryManagement_IRQn interrupt configuration
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    // BusFault_IRQn interrupt configuration
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    // UsageFault_IRQn interrupt configuration
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    // SVCall_IRQn interrupt configuration
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    // DebugMonitor_IRQn interrupt configuration
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    // PendSV_IRQn interrupt configuration
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    // SysTick_IRQn interrupt configuration
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
