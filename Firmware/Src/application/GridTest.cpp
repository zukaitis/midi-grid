#include "application/GridTest.h"

#include "grid/AdditionalButtons.h"
#include "grid/Grid.h"
#include "usb/UsbMidi.h"

#include "ticks.hpp"

namespace application
{

GridTest::GridTest( grid::Grid& grid, grid::AdditionalButtons& additionalButtons, midi::UsbMidi& usbMidi ):
    Thread( "Application_GridTest", 200, 3 ),
    grid_( grid ),
    additionalButtons_( additionalButtons ),
    usbMidi_( usbMidi )
{
    initializeGridInputHandler( grid );
    initializeAdditionalButtonInputHandler( additionalButtons );
    initializeMidiInputAvailableHandler( usbMidi );

    Start();
    Suspend();
}

void GridTest::initialize()
{
    static bool introAnimationDisplayed = false;
    if (!introAnimationDisplayed)
    {
        Resume();
        introAnimationDisplayed = true;
    }
}

void GridTest::deinitialize()
{
    grid_.turnAllLedsOff();
    Suspend();
}

void GridTest::Run()
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

    Suspend();
}

/* calculates color value according to led position */
Color GridTest::getBootAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY )
{
    Color color = {0, 0, 0};

    color.Red = ((7 - std::max( ledPositionY, static_cast<uint8_t>(7U - ledPositionX) )) * 64) / 7;
    color.Green = (abs( 7 - ledPositionX - ledPositionY ) * 64) / 7;
    color.Blue = ((7 - std::max( ledPositionX, static_cast<uint8_t>(7U - ledPositionY) )) * 64) / 7;

    return color;
}

Color GridTest::getRandomColor()
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
    int8_t partlyLitColor1 = (rand() % (64 + 32 + 1)) - 32;
    if (partlyLitColor1 < 0)
    {
        partlyLitColor1 = 0;
    }
    int8_t partlyLitColor2 = (rand() % (64 + 32 + 1)) - 32;
    if (partlyLitColor2 < 0)
    {
        partlyLitColor2 = 0;
    }

    Color color = { 0, 0, 0 };

    switch (fullyLitColor)
    {
        case kRed:
            color.Red = 64;
            color.Green = static_cast<uint8_t>(partlyLitColor1);
            color.Blue = static_cast<uint8_t>(partlyLitColor2);
            break;
        case kGreen:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = 64;
            color.Blue = static_cast<uint8_t>(partlyLitColor2);
            break;
        case kBlue:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = static_cast<uint8_t>(partlyLitColor2);
            color.Blue = 64;
            break;
        case kRedAndGreen:
            color.Red = 64;
            color.Green = 64;
            color.Blue = static_cast<uint8_t>(partlyLitColor1);
            break;
        case kRedAndBlue:
            color.Red = 64;
            color.Green = static_cast<uint8_t>(partlyLitColor1);
            color.Blue = 64;
            break;
        case kGreenAndBlue:
            color.Red = static_cast<uint8_t>(partlyLitColor1);
            color.Green = 64;
            color.Blue = 64;
            break;
        default:
            break;
    }

    return color;
}

void GridTest::handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event )
{
    if ((grid::AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_PRESSED == event.action))
    {
        switchApplication( ApplicationIndex_INTERNAL_MENU );
    }
}

void GridTest::handleGridButtonEvent( const grid::Grid::ButtonEvent event )
{
    Color color = { 0, 0, 0 };
    if (ButtonAction_PRESSED == event.action)
    {
        color = getRandomColor();
    }
    grid_.setLed( event.positionX, event.positionY, color );
}

void GridTest::handleMidiPacketAvailable()
{
    switchApplication( ApplicationIndex_LAUNCHPAD );
}

}
