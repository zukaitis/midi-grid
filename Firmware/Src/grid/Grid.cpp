#include "grid/Grid.h"
#include "grid/GridDriver.h"
#include "system/GlobalInterrupts.h"
#include "system/Time.h"

#include<math.h>

//#include "lcd/Lcd.h" // for debugging, to be removed

namespace grid
{

static const uint8_t kNumberOfPadColumns = 8;
static const uint32_t kLedFlashingPeriod = 250; // 120bpm - default flashing rate
static const uint32_t kLedPulseStepInterval = 67; // 1000ms / 15 = 66.6... ms
static const uint8_t kLedPulseStepCount = 15;

Grid::Grid( GridDriver& gridDriver, mcu::GlobalInterrupts& globalInterrupts, mcu::Time& time ) :
        gridDriver_( gridDriver ),
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

void Grid::refreshLeds() const
{
    static uint32_t ledFlashCheckTime = 0;
    static uint32_t ledPulseCheckTime = 0;

    if (time_.getSystemTick() >= ledFlashCheckTime)
    {
        static uint8_t flashColorIndex = 0;

        for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
        {
            setLedOutput(
                    flashingLed_[i].positionX,
                    flashingLed_[i].positionY,
                    flashingLed_[i].color[flashColorIndex] );
        }
        flashColorIndex = (flashColorIndex + 1) % kLedFlashingNumberOfColors;
        ledFlashCheckTime = time_.getSystemTick() + kLedFlashingPeriod; //250ms
    }

    if (time_.getSystemTick() >= ledPulseCheckTime)
    {
        static uint8_t ledPulseStepNumber = 0;

        ledPulseStepNumber = (ledPulseStepNumber + 1) % kLedPulseStepCount;

        for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
        {
            Color dimmedColor = led_[pulsingLed_[i].positionX][pulsingLed_[i].positionY].color;
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
            setLedOutput( pulsingLed_[i].positionX, pulsingLed_[i].positionY, dimmedColor );
        }
        ledPulseCheckTime = time_.getSystemTick() + kLedPulseStepInterval;
    }
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
            led_[ledPositionX][ledPositionY].color = color;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_LIGHT;
            setLedOutput( ledPositionX, ledPositionY, color );
            break;
        case LedLightingType_FLASH:
            flashingLed_[numberOfFlashingLeds_].positionX = ledPositionX;
            flashingLed_[numberOfFlashingLeds_].positionY = ledPositionY;
            //save current color to have an alternate
            flashingLed_[numberOfFlashingLeds_].color[1] = led_[ledPositionX][ledPositionY].color;
            flashingLed_[numberOfFlashingLeds_].color[0] = color;
            ++numberOfFlashingLeds_;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_FLASH;
            led_[ledPositionX][ledPositionY].color = color;
            // don't change output value, it will be set on next flash period
            break;
        case LedLightingType_PULSE:
            //save current color to have an alternate
            pulsingLed_[numberOfPulsingLeds_].positionX = ledPositionX;
            pulsingLed_[numberOfPulsingLeds_].positionY = ledPositionY;
            ++numberOfPulsingLeds_;
            led_[ledPositionX][ledPositionY].lightingType = LedLightingType_PULSE;
            led_[ledPositionX][ledPositionY].color = color;
            // don't change output value, it will be set on next pulse period
            break;
        default:
            break;
    }
}

void Grid::setLedOutput( uint8_t ledPositionX, uint8_t ledPositionY, const Color color ) const
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

}// namespace

