#include "application/grid_test/GridTest.hpp"

#include "io/grid/Grid.hpp"
#include "io/usb/UsbMidi.hpp"
#include "io/lcd/Lcd.hpp"
#include "application/images.h"

#include "ticks.hpp"

#include <cstdlib>

namespace application
{

static const lcd::Image usbLogo = { usbLogoArray, 60, 24 };

GridTest::GridTest( ApplicationController& applicationController, grid::Grid& grid, lcd::Lcd& lcd, midi::UsbMidi& usbMidi ):
    Application( applicationController ),
    grid_( grid ),
    lcd_( lcd )
{
    std::srand( static_cast<uint16_t>(freertos::Ticks::GetTicks()) ); // change seed for that extra randomness
}

void GridTest::run( ApplicationThread& thread )
{
    static bool introAnimationDisplayed = false;
    if (!introAnimationDisplayed)
    {
        displayIntroAnimation( thread );
        introAnimationDisplayed = true;
    }

    displayWaitingForMidi();
    grid_.turnAllLedsOff();

    enableGridInputHandler();
    enableAdditionalButtonInputHandler();
    enableMidiInputAvailableHandler();
}

void GridTest::handleAdditionalButtonEvent( const AdditionalButtons::Event event )
{
    if ((AdditionalButtons::internalMenuButton == event.button) && (ButtonAction_PRESSED == event.action))
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

void GridTest::displayIntroAnimation( ApplicationThread& thread )
{
    static const uint8_t totalNumberOfSteps = 8;
    static const TickType_t delayPeriod = freertos::Ticks::MsToTicks( 70 );

    grid_.turnAllLedsOff();

    for (uint8_t currentStepNumber = 0; currentStepNumber < totalNumberOfSteps; currentStepNumber++)
    {
        for (uint8_t x = 0; x <= currentStepNumber; x++)
        {
            const uint8_t y = currentStepNumber;
            grid_.setLed( x, y, getIntroAnimationColor( x, y ) );
            grid_.setLed( 7U - x, 7U - y, getIntroAnimationColor( 7U - x, 7U - y ) );
        }

        for (uint8_t y = 0; y <= currentStepNumber; y++)
        {
            const uint8_t x = currentStepNumber;
            grid_.setLed( x, y, getIntroAnimationColor( x, y ) );
            grid_.setLed( 7U - x, 7U - y, getIntroAnimationColor( 7U - x, 7U - y ) );
        }
        
        thread.DelayUntil( delayPeriod );
        grid_.turnAllLedsOff();
    }
}

/* calculates color value according to led position */
Color GridTest::getIntroAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY )
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

    const FullyLitColor fullyLitColor = static_cast<FullyLitColor>(std::rand() % kNumberOfVariants);
    int8_t partlyLitColor1 = (std::rand() % (64 + 32 + 1)) - 32;
    if (partlyLitColor1 < 0)
    {
        partlyLitColor1 = 0;
    }
    int8_t partlyLitColor2 = (std::rand() % (64 + 32 + 1)) - 32;
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

void GridTest::displayWaitingForMidi()
{
    lcd_.clear();
    lcd_.displayImage( 12, 8, usbLogo );
    lcd_.print( "Awaiting MIDI", lcd_.horizontalCenter, 40, lcd::Justification_CENTER );
}

} // namespace
