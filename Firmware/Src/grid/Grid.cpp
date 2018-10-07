#include "grid/Grid.h"
#include "grid/GridControl.h"
#include "system/GlobalInterrupts.h"
#include "system/Time.h"

#include<stdlib.h>

//#include "lcd/Lcd.h" // for debugging, to be removed

namespace grid
{

static const uint8_t kNumberOfPadColumns = 8;
static const uint32_t kLedFlashingPeriod = 250; // 120bpm - default flashing rate
static const uint32_t kLedPulseStepInterval = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t kLedPulseStepCount = 15;

Grid::Grid( GridControl& gridControl, GlobalInterrupts& globalInterrupts, Time& time ) :
        gridControl_( gridControl ),
        globalInterrupts_( globalInterrupts ),
        time_( time ),
        numberOfFlashingLeds_( 0 ),
        numberOfPulsingLeds_( 0 )
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

bool Grid::areColoursEqual( const Colour& colour1, const Colour& colour2 ) const
{
    bool equal = true;
    equal &= (colour1.Red == colour2.Red);
    equal &= (colour1.Green == colour2.Green);
    equal &= (colour1.Blue == colour2.Blue);
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

    if (gridControl_.isGridInputUpdated())
    {
        globalInterrupts_.disable();
        updateButtonColumnInput();
        gridControl_.resetGridInputUpdatedFlag();
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

Colour Grid::getLedColour( const uint8_t ledPositionX, const uint8_t ledPositionY ) const
{
    return led_[ledPositionX][ledPositionY].colour;
}

Colour Grid::getRandomColour()
{
    enum FullyLitColour
    {
        kRed = 0,
        kGreen,
        kBlue,
        kRedAndGreen,
        kRedAndBlue,
        kGreenAndBlue,
        kNumberOfVariants
    };

    const FullyLitColour fullyLitColour = static_cast<FullyLitColour>(rand() % kNumberOfVariants);
    int8_t partlyLitColour1 = (rand() % (gridControl_.ledColourIntensityMaximum + 32 + 1)) - 32;
    if (partlyLitColour1 < gridControl_.ledColourIntensityOff)
    {
        partlyLitColour1 = gridControl_.ledColourIntensityOff;
    }
    int8_t partlyLitColour2 = (rand() % (gridControl_.ledColourIntensityMaximum + 32 + 1)) - 32;
    if (partlyLitColour2 < gridControl_.ledColourIntensityOff)
    {
        partlyLitColour2 = gridControl_.ledColourIntensityOff;
    }

    Colour colour = { 0, 0, 0 };

    switch (fullyLitColour)
    {
        case kRed:
            colour.Red = gridControl_.ledColourIntensityMaximum;
            colour.Green = static_cast<uint8_t>(partlyLitColour1);
            colour.Blue = static_cast<uint8_t>(partlyLitColour2);
            break;
        case kGreen:
            colour.Red = static_cast<uint8_t>(partlyLitColour1);
            colour.Green = gridControl_.ledColourIntensityMaximum;
            colour.Blue = static_cast<uint8_t>(partlyLitColour2);
            break;
        case kBlue:
            colour.Red = static_cast<uint8_t>(partlyLitColour1);
            colour.Green = static_cast<uint8_t>(partlyLitColour2);
            colour.Blue = gridControl_.ledColourIntensityMaximum;
            break;
        case kRedAndGreen:
            colour.Red = gridControl_.ledColourIntensityMaximum;
            colour.Green = gridControl_.ledColourIntensityMaximum;
            colour.Blue = static_cast<uint8_t>(partlyLitColour1);
            break;
        case kRedAndBlue:
            colour.Red = gridControl_.ledColourIntensityMaximum;
            colour.Green = static_cast<uint8_t>(partlyLitColour1);
            colour.Blue = gridControl_.ledColourIntensityMaximum;
            break;
        case kGreenAndBlue:
            colour.Red = static_cast<uint8_t>(partlyLitColour1);
            colour.Green = gridControl_.ledColourIntensityMaximum;
            colour.Blue = gridControl_.ledColourIntensityMaximum;
            break;
        default:
            break;
    }

    return colour;
}

void Grid::refreshLeds() const
{
    static uint32_t ledFlashCheckTime = 0;
    static uint32_t ledPulseCheckTime = 0;

    if (time_.getSystemTick() >= ledFlashCheckTime)
    {
        static uint8_t flashColourIndex = 0;

        for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
        {
            setLedOutput(
                    flashingLed_[i].positionX,
                    flashingLed_[i].positionY,
                    flashingLed_[i].colour[flashColourIndex] );
        }
        flashColourIndex = (flashColourIndex + 1) % kLedFlashingNumberOfColours;
        ledFlashCheckTime = time_.getSystemTick() + kLedFlashingPeriod; //250ms
    }

    if (time_.getSystemTick() >= ledPulseCheckTime)
    {
        static uint8_t ledPulseStepNumber = 0;

        ledPulseStepNumber = (ledPulseStepNumber + 1) % kLedPulseStepCount;

        for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
        {
            Colour dimmedColour = led_[pulsingLed_[i].positionX][pulsingLed_[i].positionY].colour;
            if (ledPulseStepNumber <= 3)
            {
                // y = x / 4
                dimmedColour.Red = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
                dimmedColour.Green = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
                dimmedColour.Blue = (dimmedColour.Red * (ledPulseStepNumber + 1)) / 4;
            }
            else
            {
                // y = -x / 16
                dimmedColour.Red = (dimmedColour.Red * (19 - ledPulseStepNumber)) / 16;
                dimmedColour.Green = (dimmedColour.Green * (19 - ledPulseStepNumber)) / 16;
                dimmedColour.Blue = (dimmedColour.Blue * (19 - ledPulseStepNumber)) / 16;
            }
            setLedOutput( pulsingLed_[i].positionX, pulsingLed_[i].positionY, dimmedColour );
        }
        ledPulseCheckTime = time_.getSystemTick() + kLedPulseStepInterval;
    }
}

void Grid::setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour )
{
    setLed( ledPositionX, ledPositionY, colour, LedLightingType_LIGHT );
}

void Grid::setLed( const uint8_t ledPositionX, const uint8_t ledPositionY, const Colour colour, const LedLightingType lightingType )
{
    // remove led from flashing or pulsing list if it's in that list and proceed with setting the led
    if (LedLightingType_FLASH == led_[ledPositionX][ledPositionY].lightingType)
    {
        for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
        {
            if ((ledPositionX == flashingLed_[i].positionX) && (ledPositionY == flashingLed_[i].positionY))
            {
                // move last element into the place of element that is being removed
                flashingLed_[i] = flashingLed_[numberOfFlashingLeds_ - 1];
                numberOfFlashingLeds_--;
                break;
            }
        }
    }
    else if (LedLightingType_PULSE == led_[ledPositionX][ledPositionY].lightingType)
    {
        for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
        {
            if ((ledPositionX == pulsingLed_[i].positionX)&&(ledPositionY == pulsingLed_[i].positionY))
            {
                // move last element into the place of element that is being removed
                pulsingLed_[i] = pulsingLed_[numberOfPulsingLeds_ - 1];
                numberOfPulsingLeds_--;
                break;
            }
        }
    }

    switch (lightingType)
    {
        case LedLightingType_LIGHT:
            led_[ledPositionX][ledPositionY].colour = colour;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_LIGHT;
            setLedOutput( ledPositionX, ledPositionY, colour );
            break;
        case LedLightingType_FLASH:
            flashingLed_[numberOfFlashingLeds_].positionX = ledPositionX;
            flashingLed_[numberOfFlashingLeds_].positionY = ledPositionY;
            //save current color to have an alternate
            flashingLed_[numberOfFlashingLeds_].colour[1] = led_[ledPositionX][ledPositionY].colour;
            flashingLed_[numberOfFlashingLeds_].colour[0] = colour;
            ++numberOfFlashingLeds_;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_FLASH;
            led_[ledPositionX][ledPositionY].colour = colour;
            // don't change output value, it will be set on next flash period
            break;
        case LedLightingType_PULSE:
            //save current color to have an alternate
            pulsingLed_[numberOfPulsingLeds_].positionX = ledPositionX;
            pulsingLed_[numberOfPulsingLeds_].positionY = ledPositionY;
            ++numberOfPulsingLeds_;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_PULSE;
            led_[ledPositionX][ledPositionY].colour = colour;
            // don't change output value, it will be set on next pulse period
            break;
        default:
            break;
    }
}

void Grid::setLedOutput( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const
{
    // evaluate if led is mounted under pad (more intensity), or to illuminate directly (less intensity)
    const bool directLed = (ledPositionX >= kNumberOfPadColumns);

    if (ledPositionY >= gridControl_.numberOfHorizontalSegments)
    {
        ledPositionX += numberOfColumns;
        ledPositionY = ledPositionY % gridControl_.numberOfHorizontalSegments;
    }

    gridControl_.setLedColour( ledPositionX, ledPositionY, directLed, colour );

}

void Grid::turnAllLedsOff()
{
    gridControl_.turnAllLedsOff();

    // todo: also remove flashing, pulsing leds and reset colours to zeros
}

void Grid::updateButtonColumnInput()
{
    for (int8_t column = 0; column < numberOfColumns; column++)
    {
        // update column bits [0:3]
        if (gridControl_.isGridVerticalSegmentInputStable( column ))
        {
            // overwrite existing value with a new one
            buttonColumnInput_[column] &= 0xF0;
            buttonColumnInput_[column] |= gridControl_.getGridButtonInput( column );
        }

        // update column bits [4:7]
        if (gridControl_.isGridVerticalSegmentInputStable( column + numberOfColumns ))
        {
            // overwrite existing value with a new one
            buttonColumnInput_[column] &= 0x0F;
            buttonColumnInput_[column] |= gridControl_.getGridButtonInput( column + numberOfColumns ) << 4;
        }
    }
}

}// namespace

