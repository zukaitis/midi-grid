#include "grid/Grid.h"

//#include "lcd/Lcd.h" // for debugging, to be removed

namespace grid
{

Grid::Grid( grid_control::GridControl& gridControl ) :
        gridControl_( gridControl ),
        initialized_( false ),
        numberOfFlashingLeds_( 0 ),
        numberOfPulsingLeds_( 0 )
{
    for (uint8_t i = 0; i < grid_control::NUMBER_OF_COLUMNS; i++)
    {
        registeredButtonInput_[i] = 0x0000;
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

void Grid::enable()
{
    if (initialized_)
    {
        gridControl_.start();
    }
}

bool Grid::getButtonEvent( uint8_t& buttonPositionX, uint8_t& buttonPositionY, ButtonEvent& buttonEvent )
{
    static bool buttonChangeDetected = false;

    if (gridControl_.isGridInputUpdated() || buttonChangeDetected)
    {
        __disable_irq();
        buttonChangeDetected = false; //reset this variable every time, it will be set back if necessary
        gridControl_.resetGridInputUpdatedFlag();
        for (int8_t x = 0; x < grid_control::NUMBER_OF_COLUMNS; x++)
        {
            if (gridControl_.isGridColumnInputStable( x ))
            {
                const uint8_t buttonInput = gridControl_.getGridButtonInput( x );
                const uint8_t buttonColumnChanges = registeredButtonInput_[x] ^ buttonInput;
                if (0 != buttonColumnChanges)
                {
                    for (int8_t y = 0; y < grid_control::NUMBER_OF_ROWS; y++)
                    {
                        if (0 != ((buttonColumnChanges >> y) & 0x0001))
                        {
                            buttonEvent = static_cast<ButtonEvent>((buttonInput >> y) & 0x01);
                            registeredButtonInput_[x] ^= (1 << y); // toggle bit that was registered
                            if (x >= NUMBER_OF_COLUMNS)
                            {
                                x -= NUMBER_OF_COLUMNS;
                                y += grid_control::NUMBER_OF_ROWS;
                            }
                            buttonPositionX = x;
                            buttonPositionY = y;
                            buttonChangeDetected = true;
                            __enable_irq();
                            return true;
                        }
                    }
                }
            }
        }
    }
    __enable_irq();
    return false;
}

Colour Grid::getLedColour( const uint8_t ledPositionX, const uint8_t ledPositionY ) const
{
    return led_[ledPositionX][ledPositionY].colour;
}

void Grid::initialize()
{
    gridControl_.initialize();
    initialized_ = true;
}

void Grid::refreshLeds() const
{
    static uint32_t ledFlashCheckTime = 0;
    static uint32_t ledPulseCheckTime = 0;

    if (HAL_GetTick() >= ledFlashCheckTime)
    {
        static uint8_t flashColourIndex = 0;

        for (uint8_t i = 0; i < numberOfFlashingLeds_; i++)
        {
            setLedColour(
                    flashingLed_[i].positionX,
                    flashingLed_[i].positionY,
                    flashingLed_[i].colour[flashColourIndex] );
        }
        flashColourIndex = (flashColourIndex + 1) % LED_FLASHING_NUMBER_OF_COLOURS;
        ledFlashCheckTime = HAL_GetTick() + LED_FLASH_PERIOD_MS; //250ms
    }

    if (HAL_GetTick() >= ledPulseCheckTime)
    {
        static uint8_t ledPulseStepNumber = 0;
        Colour dimmedColour;
        ++ledPulseStepNumber;
        if (LED_PULSE_STEP_COUNT <= ledPulseStepNumber)
        {
            ledPulseStepNumber = 0;
        }

        for (uint8_t i = 0; i < numberOfPulsingLeds_; i++)
        {
            dimmedColour = led_[pulsingLed_[i].positionX][pulsingLed_[i].positionY].colour;
            if (3 >= ledPulseStepNumber)
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
            setLedColour( pulsingLed_[i].positionX, pulsingLed_[i].positionY, dimmedColour );
        }
        ledPulseCheckTime = HAL_GetTick() + LED_PULSE_STEP_PERIOD_MS;
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
            setLedColour( ledPositionX, ledPositionY, colour );
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

void Grid::setLedColour( uint8_t ledPositionX, uint8_t ledPositionY, const Colour colour ) const
{
    // evaluate if led is mounted under pad (more intensity), or to illuminate directly (less intensity)
    const bool directLed = (ledPositionX >= NUMBER_OF_PAD_COLUMNS);

    if (ledPositionY >= grid_control::NUMBER_OF_ROWS)
    {
        ledPositionX += NUMBER_OF_COLUMNS;
        ledPositionY = ledPositionY % grid_control::NUMBER_OF_ROWS;
    }

    gridControl_.setLedColour( ledPositionX, ledPositionY, directLed, colour );

}

void Grid::turnAllLedsOff()
{
    gridControl_.turnAllLedsOff();

    // todo: also remove flashing, pulsing leds and reset colours to zeros
}

}// namespace

