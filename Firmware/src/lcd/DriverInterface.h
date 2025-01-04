#pragma once

#include "types/Color.h"
#include "types/Coordinates.h"
#include <cstdint>
#include <etl/string_view.h>

namespace lcd
{

struct ImageLegacy;
class ImageMono;

struct ImageColors
{
    Color image;
    Color background;
};

class DriverInterface
{
public:
    virtual ~DriverInterface() = default;

    virtual void initialize() = 0;

    virtual void fill( const Color& color ) = 0;

    virtual void putPixel( const Coordinates& coords, const Color& color ) = 0;
    virtual void flush() = 0;
};

}  // namespace lcd
