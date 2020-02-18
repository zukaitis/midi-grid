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
    virtual void clearArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) = 0; // TODO(unknown): replace with coordinates
    virtual void displayImage( uint8_t x, uint8_t y, const Image& image ) = 0; // TODO(unknown): replace with coordinates
    virtual void putChar( uint8_t x, uint8_t y, char c ) = 0; // TODO(unknown): replace with coordinates

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint16_t numberOfTextLines() const = 0;
};

}  // namespace lcd
