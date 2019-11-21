#include "main.hpp"
#include "ticks.hpp"

#include "stm32f4xx_hal.h"

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main(void)
{
    Main& main = Main::getInstance();
    main.run();
}

Main::Main() :
        system_( mcu::System() ),
        globalInterrupts_( mcu::GlobalInterrupts() ),
        gridDriver_( hardware::grid::GridDriver() ),
        grid_( grid::Grid( gridDriver_, gridDriver_, globalInterrupts_ ) ),
        additionalButtons_( additional_buttons::AdditionalButtons( gridDriver_ ) ),
        rotaryControls_( RotaryControls( gridDriver_ ) ),
        usbMidi_( midi::UsbMidi() ),
        lcd_( lcd::Lcd() ),
        applicationController_( application::ApplicationController( additionalButtons_, grid_, rotaryControls_, usbMidi_ ) ),
        startup_( application::Startup( applicationController_, gridDriver_, lcd_, system_ ) ),
        gridTest_( application::GridTest( applicationController_, grid_, lcd_, usbMidi_ ) ),
        internalMenu_( application::InternalMenu( applicationController_, grid_, additionalButtons_, lcd_, system_ ) ),
        launchpad_( application::launchpad::Launchpad( applicationController_, grid_, additionalButtons_, rotaryControls_, lcd_, usbMidi_ ) ),
        snake_( application::Snake( applicationController_, grid_, lcd_ ) )
{
    application::Application* applicationList[application::kNumberOfApplications] = {
        NULL, // ApplicationIndex_PREVIOUS
        &startup_, // ApplicationIndex_STARTUP
        &gridTest_, // ApplicationIndex_GRID_TEST
        &internalMenu_, // ApplicationIndex_INTERNAL_MENU
        &launchpad_, // ApplicationIndex_LAUNCHPAD
        &snake_ // ApplicationIndex_SNAKE
    };

    applicationController_.initialize( applicationList );
}

void Main::run()
{
    globalInterrupts_.disable();
    system_.initialize();

    //applicationController_.selectApplication( application::ApplicationIndex_STARTUP );
    freertos::Thread::StartScheduler();
}

extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    volatile uint32_t i;
    while (true)
    {
        i++;
    }
}
