#include "main.h"
#include "stm32f4xx_hal.h"
#include "lcd/Lcd.h" // to be removed

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
        gridControl_( grid::grid_control::GridControl() ),
        grid_( grid::Grid( gridControl_ ) ),
        switches_( grid::switches::Switches( gridControl_ ) ),
        gui_( lcd::gui::Gui() ),
        usbMidi_( midi::UsbMidi() ),
        launchpad_( launchpad::Launchpad(grid_, switches_, gui_, usbMidi_) ),
        lcd_( lcd::Lcd::getInstance() )
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

    lcd_.initialize();
    lcd_.setBacklightIntensity( 55 );

    grid_.initialize(); // should replace with GridControl::initialize() and get rid of Grid::initialize method
    gridControl_.start();

    #ifdef USE_SEMIHOSTING
    printf("Semihosting output enabled\n");
    #endif
}

void ApplicationMain::run()
{
    gui_.displayConnectingImage();

    while (!isUsbConnected())
    {
        gui_.refresh();
    }

    gui_.displayWaitingForMidi();

#ifdef DISPLAY_BRIGHTNESS_LEVELS
    for (uint8_t x=0; x<8; x++)
    {
        for (uint8_t y=0; y<8; y++)
        {
            Colour colour = {static_cast<uint8_t>(x*8+y+1), 0, 0};
            grid_.setLed( x, y, colour );
        }
    }
#endif



    while (!usbMidi_.isPacketAvailable())
    {
        uint8_t button;
        ButtonEvent event;
        if (switches_.getButtonEvent( button, event ))
        {
            if ((grid::switches::kInternalMenuButton == button) && (ButtonEvent_PRESSED == event))
            {
                runInternalMenu();
            }
        }
        //randomLightAnimation();
        runGridInputTest();
        gui_.refresh();
    }

    #ifdef USE_SEMIHOSTING
    printf("Printing unacknowledged MIDI messages:\n");
    #endif

    while (1)
    {
        launchpad_.runProgram();
        // program only returns here when red button is pressed
        if (switches_.isButtonPressed( grid::switches::kInternalMenuButton ))
        {
            runInternalMenu();
        }
    }
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

void ApplicationMain::randomLightAnimation()
{
    static uint32_t newLightTime = 0;

    if (HAL_GetTick() >= newLightTime)
    {
        static uint8_t ledsChanged = 0;

        const uint8_t ledPositionX = rand() % 8;
        const uint8_t ledPositionY = rand() % 8;

        grid_.setLed(ledPositionX, ledPositionY, getRandomColour());
        newLightTime = HAL_GetTick() + 500 + rand() % 1000;
        ledsChanged++;
        if (ledsChanged > 63)
        {
            grid_.turnAllLedsOff();
            ledsChanged = 0;
        }
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

void ApplicationMain::runGridInputTest()
{
    uint8_t buttonX, buttonY;
    ButtonEvent event;

    if (grid_.getButtonEvent( buttonX, buttonY, event ))
    {
        Colour colour = { 0, 0, 0 };
        if (ButtonEvent_PRESSED == event)
        {
            colour = grid_.getRandomColour();
        }
        grid_.setLed( buttonX, buttonY, colour );
    }
}

void ApplicationMain::runInternalMenu()
{
    uint8_t buttonX, buttonY;
    int8_t rotaryStep;
    ButtonEvent event;
    midi::MidiPacket unusedInputPacket;

    grid_.discardAllPendingButtonEvents();
    switches_.discardAllPendingEvents();

    gui_.enterInternalMenu();

    grid_.turnAllLedsOff();
    const Colour red = {64U, 0U, 0U};
    grid_.setLed(7, 0, red);

    while (1)
    {
        usbMidi_.getPacket(unusedInputPacket); // check for incoming packets and discard them

        if (grid_.getButtonEvent( buttonX, buttonY, event ))
        {
            if ((7 == buttonX) && (0 == buttonY))
            {
                // reset into DFU bootloader
                resetIntoBootloader();
            }
        }

        if (switches_.getButtonEvent( buttonX,  event ))
        {
            if ((1 == buttonX) && (!event))
            {
                break; // break internal menu loop, get back to launchpad mode
            }
        }

        switches_.getRotaryEncoderEvent( buttonX, rotaryStep ); // unused atm

        //grid.refreshLeds();
        gui_.refresh();
    }
}
