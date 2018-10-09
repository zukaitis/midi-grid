#include <system/System.h>
#include "stm32f4xx_hal.h"
#include "usb/usb_device.h"

namespace mcu
{

System::System()
{
}

System::~System()
{
}

void System::initialize()
{
    HAL_Init();
    configureNvicPriorities();
    configureSystemClock();
    MX_USB_DEVICE_Init();
}

bool System::isUsbConnected()
{
    return (0 != usb_device_isUsbConnected());
}

void System::resetIntoBootloader()
{
    // write these bytes into the end of RAM, so processor would jump into bootloader after reset
    // (there is conditional in system_stm32f4xx.c that checks for this value at the beginning of a program)
    *((unsigned long *)0x2001FFF0) = 0xDEADBEEF;
    // Reset the processor
    NVIC_SystemReset();
}

void System::configureNvicPriorities()
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

void System::configureSystemClock()
{
    static RCC_OscInitTypeDef RCC_OscInitStruct;
    static RCC_ClkInitTypeDef RCC_ClkInitStruct;

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

} // namespace hal
