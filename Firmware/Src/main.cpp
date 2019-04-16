#include "main.h"
#include "ticks.hpp"

#include "stm32f4xx_hal.h"

#include <functional>
#include <algorithm>

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main(void)
{
    ApplicationMain& applicationMain = ApplicationMain::getInstance();
    applicationMain.initialize();
    applicationMain.Start();
    freertos::Thread::StartScheduler();
}

ApplicationMain::ApplicationMain() :
        Thread("main", 500, 1),
        system_( mcu::System() ),
        globalInterrupts_( mcu::GlobalInterrupts() ),
        gridDriver_( grid::GridDriver() ),
        grid_( grid::Grid( gridDriver_, globalInterrupts_ ) ),
        additionalButtons_( grid::AdditionalButtons( gridDriver_ ) ),
        rotaryControls_( grid::RotaryControls( gridDriver_ ) ),
        usbMidi_( midi::UsbMidi() ),
        lcd_( lcd::Lcd() ),
        gui_( lcd::Gui( lcd_ ) ),
        launchpad_( launchpad::Launchpad( grid_, additionalButtons_, rotaryControls_, gui_, usbMidi_ ) ),
        internalMenu_( grid_, additionalButtons_, gui_, system_, std::bind( &ApplicationMain::switchApplicationCallback, this, std::placeholders::_1 ) )
{}

ApplicationMain::~ApplicationMain()
{}

void ApplicationMain::initialize()
{
    globalInterrupts_.disable();
    system_.initialize();
}

void ApplicationMain::Run()
{
    lcd_.initialize();
    lcd_.setBacklightIntensity( 60 );

    gridDriver_.initialize();
    gridDriver_.start();

    gui_.displayConnectingImage();

    while (!system_.isUsbConnected())
    {
    }

    gui_.displayWaitingForMidi();
    displayBootAnimation();

    while (!usbMidi_.isPacketAvailable())
    {
        grid::AdditionalButtons::Event event = {};
        if (additionalButtons_.waitForEvent( event ))
        {
            if ((grid::AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_PRESSED == event.action))
            {
                runInternalMenu();

                // clear LEDs and display USB logo at the return from internal menu
                grid_.turnAllLedsOff();
                gui_.displayWaitingForMidi();
            }
        }
        runGridInputTest();
    }

    while (true)
    {
        launchpad_.runProgram();

        // program only returns here when red button is pressed
        runInternalMenu();
    }
}

void ApplicationMain::displayBootAnimation()
{
    static const uint8_t totalNumberOfSteps = 8;
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( 70 );

    grid_.turnAllLedsOff();

    for (uint8_t currentStepNumber = 0; currentStepNumber < totalNumberOfSteps; currentStepNumber++)
    {
        for (uint8_t x = 0; x <= currentStepNumber; x++)
        {
            const uint8_t y = currentStepNumber;
            grid_.setLed( x, y, getBootAnimationColor( x, y ) );
            grid_.setLed( 7U - x, 7U - y, getBootAnimationColor( 7U - x, 7U - y ) );
        }

        for (uint8_t y = 0; y <= currentStepNumber; y++)
        {
            const uint8_t x = currentStepNumber;
            grid_.setLed( x, y, getBootAnimationColor( x, y ) );
            grid_.setLed( 7U - x, 7U - y, getBootAnimationColor( 7U - x, 7U - y ) );
        }
        
        DelayUntil( delayPeriod );
        grid_.turnAllLedsOff();
    }
}

/* calculates color value according to led position */
Color ApplicationMain::getBootAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY )
{
    Color color = {0, 0, 0};

    color.Red = ((7 - std::max( ledPositionY, static_cast<uint8_t>(7U - ledPositionX) )) * 64) / 7;
    color.Green = (abs( 7 - ledPositionX - ledPositionY ) * 64) / 7;
    color.Blue = ((7 - std::max( ledPositionX, static_cast<uint8_t>(7U - ledPositionY) )) * 64) / 7;

    return color;
}

Color ApplicationMain::getRandomColor()
{
    enum FullyLitColor
    {
        kRed = 0,
        kGreen,
        kBlue,
        kRedAndGreen,
        kRedAndBlue,
        kGreenAndBlue,
        kNumberOfVariants
    };

    const FullyLitColor fullyLitColor = static_cast<FullyLitColor>(rand() % kNumberOfVariants);
    int8_t partlyLitColor1 = (rand() % (gridDriver_.ledColorIntensityMaximum + 32 + 1)) - 32;
    if (partlyLitColor1 < gridDriver_.ledColorIntensityOff)
    {
        partlyLitColor1 = gridDriver_.ledColorIntensityOff;
    }
    int8_t partlyLitColor2 = (rand() % (gridDriver_.ledColorIntensityMaximum + 32 + 1)) - 32;
    if (partlyLitColor2 < gridDriver_.ledColorIntensityOff)
    {
        partlyLitColor2 = gridDriver_.ledColorIntensityOff;
    }

    Color color = { 0, 0, 0 };

    switch (fullyLitColor)
    {
        case kRed:
            color.Red = gridDriver_.ledColorIntensityMaximum;
            color.Green = static_cast<uint8_t>(partlyLitColor1);
            color.Blue = static_cast<uint8_t>(partlyLitColor2);
            break;
        case kGreen:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = gridDriver_.ledColorIntensityMaximum;
            color.Blue = static_cast<uint8_t>(partlyLitColor2);
            break;
        case kBlue:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = static_cast<uint8_t>(partlyLitColor2);
            color.Blue = gridDriver_.ledColorIntensityMaximum;
            break;
        case kRedAndGreen:
            color.Red = gridDriver_.ledColorIntensityMaximum;
            color.Green = gridDriver_.ledColorIntensityMaximum;
            color.Blue = static_cast<uint8_t>(partlyLitColor1);
            break;
        case kRedAndBlue:
            color.Red = gridDriver_.ledColorIntensityMaximum;
            color.Green = static_cast<uint8_t>(partlyLitColor1);
            color.Blue = gridDriver_.ledColorIntensityMaximum;
            break;
        case kGreenAndBlue:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = gridDriver_.ledColorIntensityMaximum;
            color.Blue = gridDriver_.ledColorIntensityMaximum;
            break;
        default:
            break;
    }

    return color;
}

void ApplicationMain::runGridInputTest()
{
    grid::Grid::ButtonEvent event = {};

    if (grid_.waitForButtonEvent( event ))
    {
        Color color = { 0, 0, 0 };
        if (ButtonAction_PRESSED == event.action)
        {
            color = getRandomColor();
        }
        grid_.setLed( event.positionX, event.positionY, color );
    }
}

void ApplicationMain::runInternalMenu()
{
    internalMenuRunning = true;
    internalMenu_.enable();

    while (internalMenuRunning)
    {
    }

    internalMenu_.disable();
}

void ApplicationMain::switchApplicationCallback( const uint8_t applicationIndex )
{
    if (0 == applicationIndex)
    {
        internalMenuRunning = false;
    }
}

extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    volatile uint32_t i;
    while (true)
    {
        i++;
    }
}
