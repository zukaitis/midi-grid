#pragma once

#include "types/Coordinates.h"
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
    virtual void clearArea( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 ) = 0;
    virtual void displayImage( uint8_t x, uint8_t y, const Image& image) = 0;
    virtual void displayProgressArc( uint8_t x, uint8_t y, uint8_t position ) = 0;
    virtual void initialize() = 0;
    virtual void print( const char* string, uint8_t x, uint8_t y, Justification justification = Justification::LEFT ) = 0;
    //virtual void print( const char* const string, const uint8_t y, const Justification justification ) = 0;
    virtual void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y, Justification justification = Justification::LEFT ) = 0;
    virtual void setBacklightIntensity( uint8_t intensity ) = 0;

    virtual uint16_t right() const = 0;
    virtual uint8_t maximumBacklightIntensity() const = 0;
    virtual uint8_t horizontalCenter() const = 0; // TODO(unknown): remove
};

}  // namespace lcd
