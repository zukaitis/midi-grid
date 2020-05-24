#pragma once

#include "types/Coordinates.h"
#include "types/Color.h"

namespace lcd
{

class DrawInterface
{
public:
    virtual ~DrawInterface() = default;

    virtual void line( const Coordinates& point1, const Coordinates& point2, const Color& color ) = 0;
    virtual void arc( const Coordinates& center, uint16_t innerRadius, uint16_t outerRadius,
        uint16_t startAngle, uint16_t endAngle, const Color& color ) = 0;
    virtual void circle( const Coordinates& center, uint16_t radius, const Color& color ) = 0;
    virtual void halfCircleLeft( const Coordinates& center, uint16_t radius, const Color& color ) = 0;
    virtual void halfCircleRight( const Coordinates& center, uint16_t radius, const Color& color ) = 0;
};

}  // namespace lcd
