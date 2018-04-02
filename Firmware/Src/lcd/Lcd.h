/*
 * Nokia5110_LCD Library
 *
 * This library is used to control Nokia's 5110 LCD's on STM32 devices.
 * It was built on STM's HAL and intends to offer an easy and fast way to use 5110 using HAL's GPIOs.
 * It's also based on two other 5110 libraries:
 *  1) Tilen Majerle's
 * 	   @website	http://stm32f4-discovery.com
 *	   @link	http://stm32f4-discovery.com/pcd8544-nokia-33105110-lcd-stm32f429-discovery-library/
 * 	   This library was build for STM devices, but it doesn't run on HAL.
 * 	   Tilen also has a great site about STM32.
 *
 *  2) RinkyDinkElectronic's
 * 	   @website http://www.RinkyDinkElectronics.com/
 * 	   This library was built to control 5110 for Arduino.
 *
 * I've studied both libraries and used many things of them. Some functions are exactly the same but
 * there are modified functions too.
 *
 * This library is an unfinished job (but it works!). Some things that might be added in the future:
 *  1) The rest of the functions of both libraries mentioned above
 *  2) ?
 *
 * Feel free to use this library and modify it. :D
 *
 * ----- How to use this library? -----
 * This library was built on HAL for STM32 devices, so HAL is necessary. It's recommended to use it
 * with STM32CubeMX.
 *
 * Steps to use this library:
 * 1) Import this library into your code (!)
 *
 * 2) Configure your pins using set function: LCD_setRST(PORT, PIN)
 * 						   					  LCD_setCE(PORT, PIN)
 * 						   					  LCD_setDC(PORT, PIN)
 * 						   					  LCD_setDIN(PORT, PIN)
 * 						   					  LCD_setCLK(PORT, PIN)
 *
 * 3) Call LCD_init() to initialize the LCD
 * Now the display is initialized and ready to use.
 *
 * Example:
 * 4) Call LCD_print("Hello World", 0, 0)
 *
 * --------------------
 * Author: Caio Rodrigo
 * Github: https://github.com/Zeldax64?tab=repositories
 * --------------------
 */

#ifndef nokia5110_LCD
#define nokia5110_LCD

#include "stm32f4xx_hal.h"

#include "lcd/font.h"
#include "lcd/images.h"

#include "lcd/LcdControl.h"

namespace lcd
{

enum Justification
{
    Justification_LEFT,
    Justification_RIGHT,
    Justification_CENTER
};

struct Image
{
    const uint8_t* image;
    uint16_t arraySize;
    uint8_t width;
    uint8_t height;
};

static const Image usbLogo = {usbLogoArray, 336, 84, 32};

static const Image usbSymbolSmall = { usbSymbolSmallArray, 9, 9, 8 };
static const Image midiSymbolSmall = { midiSymbolSmallArray, 8, 8, 8 };
static const Image arrowSmallUp = { arrowSmallUpArray, 5, 5, 8 };
static const Image arrowSmallDown = { arrowSmallDownArray, 5, 5, 8 };

static const Image play = { playArray, 32, 16, 16 };
static const Image recordingOn = { recordingOnArray, 32, 16, 16 };
static const Image sessionRecordingOn = { sessionRecordingOnArray, 32, 16, 16 };
static const Image nudgeDownInactive = { nudgeDownInactiveArray, 10, 10, 8};
static const Image nudgeDownActive = { nudgeDownActiveArray, 10, 10, 8};
static const Image nudgeUpInactive = { nudgeUpInactiveArray, 10, 10, 8};
static const Image nudgeUpActive = { nudgeUpActiveArray, 10, 10, 8};

static const Image digitBig[10] = {
        { DIGITS_BIG[0], 24, 12, 16 }, { DIGITS_BIG[1], 24, 12, 16 }, { DIGITS_BIG[2], 24, 12, 16 }, { DIGITS_BIG[3], 24, 12, 16 },
        { DIGITS_BIG[4], 24, 12, 16 }, { DIGITS_BIG[5], 24, 12, 16 }, { DIGITS_BIG[6], 24, 12, 16 }, { DIGITS_BIG[7], 24, 12, 16 },
        { DIGITS_BIG[8], 24, 12, 16 }, { DIGITS_BIG[9], 24, 12, 16 }
};

static const uint8_t WIDTH = 84;
static const uint8_t HEIGHT = 48;
static const uint8_t NUMBER_OF_LINES = 6;

class Lcd
{
public:

    void clear();
    void clearArea(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2);
    void refresh();
    void print(const char *string, const uint8_t x, const uint8_t y, const Justification justification);
    void print(const char *string, uint8_t x, const uint8_t y);
    void printNumberInBigDigits(const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification);
    void printNumberInBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits );
    void displayImage(const uint8_t x, const uint8_t y, const Image image);

    void initialize();

    static Lcd& getInstance()
    {
        static Lcd instance;
        return instance;
    }

    ~Lcd();

private:

    Lcd();

    lcd_control::LcdControl lcdControl;

    void putChar( const uint8_t x, const uint8_t y, const char c );
    bool updateRequired = false;

    uint8_t lcdBuffer[NUMBER_OF_LINES][WIDTH];
};

} // namespace

#endif
