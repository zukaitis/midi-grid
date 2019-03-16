#include "grid/Grid.h"
#include "grid/GridDriver.h"
#include "system/GlobalInterrupts.h"
#include "system/Time.h"

#include "ticks.hpp"
#include <math.h>

//#include "lcd/Lcd.h" // for debugging, to be removed

namespace grid
{

static const uint8_t kNumberOfPadColumns = 8;
static const uint32_t kLedFlashingPeriod = 250; // 120bpm - default flashing rate
static const uint32_t kLedPulseStepInterval = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t kLedPulseStepCount = 15;

Grid::Grid( GridDriver& gridDriver, mcu::GlobalInterrupts& globalInterrupts, mcu::Time& time ) :
        gridDriver_( gridDriver ),
        ledOutput_( GridLedOutput( gridDriver ) ),
        flashingLeds_( ledOutput_ ),
        pulsingLeds_( ledOutput_ ),
        globalInterrupts_( globalInterrupts ),
        time_( time )
{
    for (uint8_t index = 0; index < numberOfColumns; index++)
    {
        buttonColumnInput_[index] = 0x00;
        registeredButtonColumnInput_[index] = 0x00;
    }
}

Grid::~Grid()
{
}

bool Grid::areColorsEqual( const Color& color1, const Color& color2 ) const
{
    bool equal = true;
    equal &= (color1.Red == color2.Red);
    equal &= (color1.Green == color2.Green);
    equal &= (color1.Blue == color2.Blue);
    return equal;
}

void Grid::discardAllPendingButtonEvents()
{
    uint8_t unusedUnsignedChar;
    ButtonEvent unusedEvent;

    // call method while it has pending events
    while (getButtonEvent( unusedUnsignedChar, unusedUnsignedChar, unusedEvent ))
    {
    }
}

bool Grid::getButtonEvent( uint8_t& buttonPositionX, uint8_t& buttonPositionY, ButtonEvent& buttonEvent )
{
    static bool buttonChangeDetected = false;

    bool eventAvailable = false;

    if (gridDriver_.isGridInputUpdated())
    {
        // interrupts are disabled, so that active buffer wouldn't change during reading
        globalInterrupts_.disable();
        updateButtonColumnInput();
        gridDriver_.resetGridInputUpdatedFlag();
        globalInterrupts_.enable();
        buttonChangeDetected = true;
    }

    if (buttonChangeDetected)
    {
        buttonChangeDetected = false; //reset this variable every time, it will be set back on if necessary

        for (uint8_t x = 0; x < numberOfColumns; x++)
        {
            const uint8_t columnChanges = buttonColumnInput_[x] ^ registeredButtonColumnInput_[x];
            if (0 != columnChanges)
            {
                for (uint8_t y = 0; y < numberOfRows; y++)
                {
                    if (0 != ((columnChanges >> y) & 0x01))
                    {
                        buttonEvent = static_cast<ButtonEvent>((buttonColumnInput_[x] >> y) & 0x01);
                        buttonPositionX = x;
                        buttonPositionY = y;
                        registeredButtonColumnInput_[x] ^= (1 << y); // toggle bit that was registered
                        buttonChangeDetected = true;
                        eventAvailable = true;
                        x = numberOfColumns; // break out of the first loop
                        y = numberOfRows; // break out of the second loop
                    }
                }
            }
        }
    }
    return eventAvailable;
}

Color Grid::getLedColor( const uint8_t ledPositionX, const uint8_t ledPositionY ) const
{
    return led_[ledPositionX][ledPositionY].color;
}

Color Grid::getRandomColor()
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

void Grid::setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color )
{
    setLed( ledPositionX, ledPositionY, color, LedLightingType_LIGHT );
}

void Grid::setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color, const LedLightingType lightingType )
{
    // remove led from flashing or pulsing list if it's in that list and proceed with setting the led
    if (LedLightingType_FLASH == led_[ledPositionX][ledPositionY].lightingType)
    {
        flashingLeds_.remove( ledPositionX, ledPositionY );
    }
    else if (LedLightingType_PULSE == led_[ledPositionX][ledPositionY].lightingType)
    {
        pulsingLeds_.remove( ledPositionX, ledPositionY );
    }

    switch (lightingType)
    {
        case LedLightingType_LIGHT:
            ledOutput_.set( ledPositionX, ledPositionY, color );
            break;
        case LedLightingType_FLASH:
            flashingLeds_.add( ledPositionX, ledPositionY, led_[ledPositionX][ledPositionY].color, color );
            break;
        case LedLightingType_PULSE:
            pulsingLeds_.add( ledPositionX, ledPositionY, color );
            break;
        default:
            break;
    }

    led_[ledPositionX][ledPositionY].lightingType = lightingType;
    led_[ledPositionX][ledPositionY].color = color;
}

void Grid::turnAllLedsOff()
{
    gridDriver_.turnAllLedsOff();

    // todo: also remove flashing, pulsing leds and reset colors to zeros
}

void Grid::updateButtonColumnInput()
{
    for (int8_t column = 0; column < numberOfColumns; column++)
    {
        // update column bits [0:3]
        if (gridDriver_.isGridVerticalSegmentInputStable( column ))
        {
            // overwrite existing value with a new one
            buttonColumnInput_[column] &= 0xF0;
            buttonColumnInput_[column] |= gridDriver_.getGridButtonInput( column );
        }

        // update column bits [4:7]
        if (gridDriver_.isGridVerticalSegmentInputStable( column + numberOfColumns ))
        {
            // overwrite existing value with a new one
            buttonColumnInput_[column] &= 0x0F;
            buttonColumnInput_[column] |= gridDriver_.getGridButtonInput( column + numberOfColumns ) << 4;
        }
    }
}

GridLedOutput::GridLedOutput( GridDriver& gridDriver ):
        gridDriver_( gridDriver )
{
}

GridLedOutput::~GridLedOutput()
{
}

void GridLedOutput::set( uint8_t ledPositionX, uint8_t ledPositionY, const Color color ) const
{
    // evaluate if led is mounted under pad (more intensity), or to illuminate directly (less intensity)
    const bool directLed = (ledPositionX >= kNumberOfPadColumns);

    if (ledPositionY >= gridDriver_.numberOfHorizontalSegments)
    {
        ledPositionX += numberOfColumns;
        ledPositionY = ledPositionY % gridDriver_.numberOfHorizontalSegments;
    }

    gridDriver_.setLedColor( ledPositionX, ledPositionY, directLed, color );
}

FlashingLeds::FlashingLeds( GridLedOutput& gridLedOutput ):
        Timer( cpp_freertos::Ticks::MsToTicks( kLedFlashingPeriod ) ),
        ledOutput_( gridLedOutput ),
        numberOfFlashingLeds_( 0 )
{
}

FlashingLeds::~FlashingLeds()
{
}

void FlashingLeds::Run()
{
    static uint8_t flashColorIndex = 0;

    for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
    {
        ledOutput_.set(led_[i].positionX, led_[i].positionY, led_[i].color[flashColorIndex] );
    }
    flashColorIndex = (flashColorIndex + 1) % 2;
}

void FlashingLeds::add( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color1, const Color color2 )
{
    led_[numberOfFlashingLeds_].positionX = ledPositionX;
    led_[numberOfFlashingLeds_].positionY = ledPositionY;
    led_[numberOfFlashingLeds_].color[0] = color1;
    led_[numberOfFlashingLeds_].color[1] = color2;
    numberOfFlashingLeds_++;

    if (1 == numberOfFlashingLeds_)
    {
        Start();
    }
    // don't change output value, it will be set on next flash period
}

void FlashingLeds::remove( const uint8_t ledPositionX, const uint8_t ledPositionY )
{
    for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
    {
        if ((ledPositionX == led_[i].positionX) && (ledPositionY == led_[i].positionY))
        {
            // move last element into the place of element that is being removed
            led_[i] = led_[numberOfFlashingLeds_ - 1];
            numberOfFlashingLeds_--;

            if (0 == numberOfFlashingLeds_)
            {
                Stop();
            }
            break;
        }
    }
}

PulsingLeds::PulsingLeds( GridLedOutput& gridLedOutput ):
        Timer( cpp_freertos::Ticks::MsToTicks( kLedPulseStepInterval ) ),
        ledOutput_( gridLedOutput ),
        numberOfPulsingLeds_( 0 )
{
}

PulsingLeds::~PulsingLeds()
{
}

void PulsingLeds::Run()
{
    static uint8_t ledPulseStepNumber = 0;

    ledPulseStepNumber = (ledPulseStepNumber + 1) % kLedPulseStepCount;

    for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
    {
        Color dimmedColor = led_[i].color;
        if (ledPulseStepNumber <= 3)
        {
            // y = x / 4
            dimmedColor.Red = (dimmedColor.Red * (ledPulseStepNumber + 1)) / 4;
            dimmedColor.Green = (dimmedColor.Red * (ledPulseStepNumber + 1)) / 4;
            dimmedColor.Blue = (dimmedColor.Red * (ledPulseStepNumber + 1)) / 4;
        }
        else
        {
            // y = -x / 16
            dimmedColor.Red = (dimmedColor.Red * (19 - ledPulseStepNumber)) / 16;
            dimmedColor.Green = (dimmedColor.Green * (19 - ledPulseStepNumber)) / 16;
            dimmedColor.Blue = (dimmedColor.Blue * (19 - ledPulseStepNumber)) / 16;
        }
        ledOutput_.set( led_[i].positionX, led_[i].positionY, dimmedColor );
    }
}

void PulsingLeds::add( const uint8_t ledPositionX, const uint8_t ledPositionY, const Color color )
{
    //save current color to have an alternate
    led_[numberOfPulsingLeds_].positionX = ledPositionX;
    led_[numberOfPulsingLeds_].positionY = ledPositionY;
    led_[numberOfPulsingLeds_].color = color;
    numberOfPulsingLeds_++;

    if (1 == numberOfPulsingLeds_)
    {
        Start();
    }
    // don't change output value, it will be set on next pulse period
}

void PulsingLeds::remove( const uint8_t ledPositionX, const uint8_t ledPositionY )
{
    for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
    {
        if ((ledPositionX == led_[i].positionX)&&(ledPositionY == led_[i].positionY))
        {
            // move last element into the place of element that is being removed
            led_[i] = led_[numberOfPulsingLeds_ - 1];
            numberOfPulsingLeds_--;

            if (0 == numberOfPulsingLeds_)
            {
                Stop();
            }
            break;
        }
    }
}

}// namespace

