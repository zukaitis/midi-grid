#include "main.hpp"
#include <freertos/ticks.hpp>

#include "stm32f4xx_hal.h"

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main()
{
    Main& main = Main::getInstance();
    main.run();
}

Main::Main() :
        system_( mcu::System() ),
        globalInterrupts_( mcu::GlobalInterrupts() ),
        gridDriver_( hardware::grid::GridDriver() ),
        gridContainer_( grid::GridContainer( &gridDriver_, &gridDriver_, &globalInterrupts_ ) ),
        additionalButtons_( additional_buttons::AdditionalButtons( &gridDriver_ ) ),
        rotaryControls_( rotary_controls::RotaryControls( &gridDriver_ ) ),
        usbMidi_( midi::UsbMidi() ),
        lcdSpi_( hardware::lcd::Spi() ),
        backlightDriver_( hardware::lcd::BacklightDriver() ),
        lcdContainer_( &lcdSpi_, &backlightDriver_ ),
        testing_( &gridDriver_ ),
        applicationController_( application::ApplicationController(
            &additionalButtons_, &gridContainer_.getGrid(), &rotaryControls_, &usbMidi_ ) ),
        startup_( applicationController_, gridDriver_, lcdContainer_.getLcd(), system_ ),
        gridTest_( applicationController_, gridContainer_.getGrid(), lcdContainer_.getLcd(), usbMidi_ ),
        internalMenu_( applicationController_, gridContainer_.getGrid(), additionalButtons_,
            lcdContainer_.getLcd(), system_ ),
        launchpad_( applicationController_, gridContainer_.getGrid(), additionalButtons_, rotaryControls_, lcdContainer_.getLcd(), usbMidi_,
            &system_ ),
        snake_( applicationController_, gridContainer_.getGrid(), lcdContainer_.getLcd() )
{
    etl::array<application::Application*, application::kNumberOfApplications> applicationList = {
        nullptr, // ApplicationIndex_PREVIOUS
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
