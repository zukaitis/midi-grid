#pragma once

#include "types/Color.h"
#include "types/Coordinates.h"
#include <cstdint>

namespace lcd
{

class ImageInterface
{
public:
    virtual ~ImageInterface() = default;

    virtual void createNew( uint16_t width, uint16_t height ) {};
    virtual void createNew( uint16_t width, uint16_t height, const Color& fill ) {};
    virtual void createNew() {};
    virtual void createNew( const Color& fill ) {};

    virtual void clear() {};
    virtual void fill( const Color& color ) {};

    virtual void endDrawing() {};
    virtual bool isBeingDrawn() { return false; };
    virtual void display( const Coordinates& coords ) {};
    virtual void display() {};

    virtual uint16_t width() { return 0; };
    virtual uint16_t height() { return 0; };

    virtual bool putPixel( const Coordinates& coords, uint8_t colorIndex ) { return false; };
    virtual uint8_t assignColorIndex( const Color& color ) { return 0; };
    virtual uint8_t getBackgroundColorIndex() { return 0; };
};

}  // namespace lcd
