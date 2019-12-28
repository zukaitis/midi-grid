#pragma once

#include <cstdint>

namespace lcd
{

struct Image
{
    const uint8_t* data;
    uint8_t width;
    uint8_t height;
};

enum class Justification
{
    LEFT,
    RIGHT,
    CENTER
};

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum class FontSize
{
    PT1,
    PT2,
    PT3
};

class LcdInterface
{
public:
    virtual ~LcdInterface() = default;

    virtual void clear() = 0;
    virtual void clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 ) = 0;
    virtual void displayImage( const uint8_t x, const uint8_t y, const Image image) = 0;
    virtual void displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position ) = 0;
    virtual void initialize() = 0;
    virtual void print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification = Justification::LEFT ) = 0;
    //virtual void print( const char* const string, const uint8_t y, const Justification justification ) = 0;
    virtual void printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification = Justification::LEFT ) = 0;
    virtual void setBacklightIntensity( const uint8_t intensity ) = 0;

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint16_t numberOfTextLines() const = 0;
    virtual uint8_t maximumBacklightIntensity() const = 0;
    virtual uint8_t horizontalCenter() const = 0; // TODO: remove
};

}