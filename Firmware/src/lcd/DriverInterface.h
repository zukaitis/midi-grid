#pragma once

#include "etl/string_view.h"
#include "types/Coordinates.h"
#include <cstdint>

namespace lcd
{

struct Image;
class Format;

class DriverInterface
{
public:
    virtual ~DriverInterface() = default;

    virtual void initialize() = 0;

    virtual void clear() = 0;
    virtual void clearArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) = 0;
    virtual void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) = 0;
    virtual void displayImage( uint8_t x, uint8_t y, const Image& image ) = 0; // TODO(unknown): replace with coordinates
    virtual void putString( const etl::string_view& string, const Coordinates& coords ) = 0;
    virtual void putString( const etl::string_view& string, const Coordinates& coords, const Format& format ) = 0;

    virtual uint16_t width() const = 0;
    virtual uint16_t height() const = 0;
    virtual uint16_t numberOfTextLines() const = 0;
};

}  // namespace lcd
