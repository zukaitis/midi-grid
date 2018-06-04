#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "lcd/font.h"
#include "lcd/images.h"
#include "lcd/progressArc.h"

#include "lcd/LcdControl.h"

#include "stm32f4xx_hal.h"

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

static const Image usbLogo = {usbLogoArray, 180, 60, 24};

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
static const uint8_t NUMBER_OF_PROGRESS_ARC_POSITIONS = NUMBER_OF_ARC_POSITIONS;

class Lcd
{
public:

    static inline Lcd& getInstance()
    {
        static Lcd instance;
        return instance;
    }

    ~Lcd();

    void clear();
    void clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 );
    void displayImage( const uint8_t x, const uint8_t y, const Image image);
    void displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position );
    void initialize();
    void print( const char *string, uint8_t x, const uint8_t y );
    void print( const char *string, const uint8_t x, const uint8_t y, const Justification justification );
    void printNumberInBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits );
    void printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification );
    void refresh();
    void setBacklightIntensity(const uint8_t intensity);

private:
    Lcd();

    void putChar( const uint8_t x, const uint8_t y, const char c );

    lcd_control::LcdControl lcdControl;

    uint8_t appointedBacklightIntensity_ = 0;
    uint8_t currentBacklightIntensity_ = 0;
    uint8_t lcdBuffer_[NUMBER_OF_LINES][WIDTH];
    bool updateRequired_ = false;
};

} // namespace

#endif // LCD_LCD_H_
