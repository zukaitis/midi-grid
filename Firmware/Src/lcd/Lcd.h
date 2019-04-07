#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include "lcd/Backlight.h"
#include "lcd/LcdDriver.h"

#include "thread.hpp"

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

class Lcd: private freertos::Thread
{
public:
    Lcd();
    ~Lcd();

    virtual void Run();

    void clear();
    void clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 );
    void displayImage( const uint8_t x, const uint8_t y, const Image image);
    void displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position );
    void initialize();
    void print( const char* string, uint8_t x, const uint8_t y );
    void print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification );
    void printNumberInBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits );
    void printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification );
    void setBacklightIntensity( const uint8_t intensity );
    void updateBacklightIntensity();
    void updateScreenContent();

    const uint8_t numberOfProgressArcPositions;

    static const uint8_t width = 84;
    static const uint8_t height = 48;
    static const uint8_t numberOfLines = 6;
    static const uint8_t numberOfCharactersInLine = 14;

private:
    void putChar( const uint8_t x, const uint8_t y, const char c );

    Backlight backlight_;
    LcdDriver lcdDriver_;

    uint8_t appointedBacklightIntensity_;
    uint8_t currentBacklightIntensity_;
    uint8_t lcdBuffer_[numberOfLines][width];
    bool updateRequired_;
};

} // namespace

#endif // LCD_LCD_H_
