#pragma once

#include "lcd/Format.h"
#include "types/Coordinates.h"
#include <cstdint>
#include <etl/string_view.h>
#include <etl/to_string.h>

namespace lcd
{

class Font;
class Image;

namespace image
{
extern const Image& circle_23x23;
extern const Image& circle_empty_23x23;
extern const Image& down_arrow_9x23;
extern const Image& midi_23x23;
extern const Image& triangle_21x23;
extern const Image& up_arrow_9x23;
extern const Image& usb_41x23;
extern const Image& usb_240x107;
}

struct ImageLegacy
{
    const uint8_t* data;
    uint8_t width;
    uint8_t height;
};

class LcdInterface
{
public:
    virtual ~LcdInterface() = default;

    virtual void setBackgroundColor( const Pixel& color ) = 0;
    virtual void clear() = 0;
    virtual void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) = 0;
    virtual void fillArea( const Coordinates& corner1, const Coordinates& corner2, const Pixel& color ) = 0;

    virtual void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image ) = 0;
    virtual void displayImage( const Coordinates& coords, const Image& image, const Pixel& color ) = 0;
    virtual void displayProgressArc( uint8_t x, uint8_t y, uint8_t position ) = 0;
    virtual void initialize() = 0;
    virtual void print( const etl::string_view& string, uint8_t x, uint8_t y, Justification justification ) = 0;
    virtual void print( const etl::string_view& string, uint8_t y, Justification justification ) = 0;
    virtual void print( const etl::string_view& string, uint8_t x, uint8_t y ) = 0;
    virtual void print( const etl::string_view& string, const Coordinates& coords, const Format& format ) = 0;
    virtual void print( const etl::string_view& string, uint8_t y, const Format& format ) = 0;
    virtual void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y, Justification justification ) = 0;
    virtual void printNumberInBigDigits( uint16_t number, uint8_t y, Justification justification ) = 0;
    virtual void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y ) = 0;
    virtual void setBacklightIntensity( uint8_t intensity ) = 0;

    virtual uint8_t maximumBacklightIntensity() const = 0;
    virtual uint16_t line( uint8_t lineNumber ) const = 0;

    virtual void release() = 0;
};

}  // namespace lcd
