#pragma once

#include "lcd/Format.h"
#include "types/Coordinates.h"
#include <cstdint>
#include <etl/string_view.h>

namespace lcd
{

struct ImageLegacy;
class Image;

struct ImageColors
{
    Pixel image;
    Pixel background;
};

class DriverInterface
{
public:
    virtual ~DriverInterface() = default;

    virtual void initialize() = 0;

    virtual void clear() = 0;
    virtual void clearArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) = 0;
    virtual void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) = 0;
    virtual void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image ) = 0; // TODO(unknown): replace with coordinates
    virtual void putImage( const Coordinates& coords, const Image& image, const ImageColors& colors ) = 0;
    virtual void putString( const etl::string_view& string, const Coordinates& coords ) = 0;
    virtual void putString( const etl::string_view& string, const Coordinates& coords, const Format& format ) = 0;

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint16_t numberOfTextLines() const = 0;
};

}  // namespace lcd
