#pragma once

#include "types/Coordinates.h"
#include "lcd/Format.h"  // TODO: replace with Color afterwards

namespace lcd
{

class DrawInterface
{
public:
    virtual ~DrawInterface() = default;

    virtual void line( const Coordinates& point1, const Coordinates& point2, const Pixel& color ) = 0;
    virtual void arc( const Coordinates& center, uint16_t innerRadius, uint16_t outerRadius,
        uint16_t startAngle, uint16_t endAngle, const Pixel& color ) = 0;
};

}  // namespace lcd
