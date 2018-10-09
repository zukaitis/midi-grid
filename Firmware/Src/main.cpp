#include "main.h"

#include <math.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main(void)
{
    ApplicationMain& applicationMain = ApplicationMain::getInstance();
    applicationMain.initialize();
    applicationMain.run(); // doesn't return
}

ApplicationMain::ApplicationMain() :
        system_( mcu::System() ),
        globalInterrupts_( mcu::GlobalInterrupts() ),
        time_( mcu::Time() ),
        gridDriver_( grid::GridDriver() ),
        grid_( grid::Grid( gridDriver_, globalInterrupts_, time_ ) ),
        switches_( grid::Switches( gridDriver_, time_ ) ),
        usbMidi_( midi::UsbMidi() ),
        lcd_( lcd::Lcd( time_ ) ),
        gui_( lcd::Gui( lcd_, time_ ) ),
        launchpad_( launchpad::Launchpad( grid_, switches_, gui_, usbMidi_ ) )
{}

ApplicationMain::~ApplicationMain()
{}

void ApplicationMain::initialize()
{

    system_.initialize();

    #ifdef USE_SEMIHOSTING
    initialise_monitor_handles(); // enable semihosting
    #endif

    lcd_.initialize();
    lcd_.setBacklightIntensity( 55 );

    gridDriver_.initialize();
    gridDriver_.start();

    #ifdef USE_SEMIHOSTING
    printf("Semihosting output enabled\n");
    #endif
}

void ApplicationMain::run()
{
    gui_.displayConnectingImage();

    while (!system_.isUsbConnected())
    {
        gui_.refresh();
    }

    gui_.displayWaitingForMidi();

#ifdef DISPLAY_BRIGHTNESS_LEVELS
    for (uint8_t x=0; x<8; x++)
    {
        for (uint8_t y=0; y<8; y++)
        {
            Color color = {static_cast<uint8_t>(x*8+y+1), 0, 0};
            grid_.setLed( x, y, color );
        }
    }
#endif

    while(!displayBootAnimation())
    {
    }

    while (!usbMidi_.isPacketAvailable())
    {
        uint8_t button;
        ButtonEvent event;
        if (switches_.getButtonEvent( button, event ))
        {
            if ((switches_.internalMenuButton == button) && (ButtonEvent_PRESSED == event))
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
        if (switches_.isButtonPressed( switches_.internalMenuButton ))
        {
            runInternalMenu();
        }
    }
}



void ApplicationMain::randomLightAnimation()
{
    static uint32_t newLightTime = 0;

    if (time_.getSystemTick() >= newLightTime)
    {
        static uint8_t ledsChanged = 0;

        const uint8_t ledPositionX = rand() % 8;
        const uint8_t ledPositionY = rand() % 8;

        grid_.setLed(ledPositionX, ledPositionY, getRandomColor());
        newLightTime = time_.getSystemTick() + 500 + rand() % 1000;
        ledsChanged++;
        if (ledsChanged > 63)
        {
            grid_.turnAllLedsOff();
            ledsChanged = 0;
        }
    }
}

bool ApplicationMain::displayBootAnimation()
{
    bool animationEnded = false;

    static uint32_t stepChangeTime = 0;
    static uint8_t currentStepNumber = 0;
    const uint8_t totalNumberOfSteps = 8;

    if (time_.getSystemTick() >= stepChangeTime)
    {
        grid_.turnAllLedsOff();

        if (currentStepNumber < totalNumberOfSteps)
        {
            for (uint8_t x = 0; x <= currentStepNumber; x++)
            {
                const uint8_t y = currentStepNumber;
                grid_.setLed( x, y, getBootAnimationColor(x, y));
                grid_.setLed( 7-x, 7-y, getBootAnimationColor(7-x, 7-y));
            }

            for (uint8_t y = 0; y < currentStepNumber; y++)
            {
                const uint8_t x = currentStepNumber;
                grid_.setLed( x, y, getBootAnimationColor(x, y));
                grid_.setLed( 7-x, 7-y, getBootAnimationColor(7-x, 7-y));
            }

            currentStepNumber++;
        }
        else
        {
            animationEnded = true;
        }

        stepChangeTime = time_.getSystemTick() + 70; // step each 70ms
    }

    return animationEnded;
}

/* calculates color value according to led position */
Color ApplicationMain::getBootAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY )
{
    Color color = {0, 0, 0};

    color.Red = ((7 - MAX( ledPositionY, 7 - ledPositionX )) * 64) / 7;
    color.Green = (abs( 7 - ledPositionX - ledPositionY ) * 64) / 7;
    color.Blue = ((7 - MAX( ledPositionX, 7 - ledPositionY )) * 64) / 7;

    return color;
}

void ApplicationMain::runGridInputTest()
{
    uint8_t buttonX, buttonY;
    ButtonEvent event;

    if (grid_.getButtonEvent( buttonX, buttonY, event ))
    {
        Color color = { 0, 0, 0 };
        if (ButtonEvent_PRESSED == event)
        {
            color = grid_.getRandomColor();
        }
        grid_.setLed( buttonX, buttonY, color );
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
    const Color red = {64U, 0U, 0U};
    grid_.setLed(7, 0, red);

    while (1)
    {
        usbMidi_.getPacket(unusedInputPacket); // check for incoming packets and discard them

        if (grid_.getButtonEvent( buttonX, buttonY, event ))
        {
            if ((7 == buttonX) && (0 == buttonY))
            {
                // reset into DFU bootloader
                system_.resetIntoBootloader();
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
