#pragma once

#include <cstdint>

namespace lcd
{

struct Image;

class DriverInterface
{
public:
    virtual ~DriverInterface() = default;

    virtual void initialize() = 0;

    virtual void clear() = 0;
    virtual void clearArea( const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2 ) = 0; // TODO replace with coordinates
    virtual void displayImage( const uint8_t x, const uint8_t y, const Image& image ) = 0; // TODO replace with coordinates
    virtual void putChar( const uint8_t x, const uint8_t y, const char c ) = 0; // TODO replace with coordinates

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint16_t numberOfTextLines() const = 0;
};

}
