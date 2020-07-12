#pragma once

#include "lcd/ImageMono.h"
#include "types/Coordinates.h"
#include "types/Color.h"

namespace lcd
{

class ShapesInterface
{
public:
    virtual ~ShapesInterface() = default;

    virtual void drawLine( const Coordinates& point1, const Coordinates& point2, const Color& color ) {};
    virtual void drawArc( const Coordinates& center, uint16_t innerRadius, uint16_t outerRadius,
        uint16_t startAngle, uint16_t endAngle, const Color& color ) {};
    virtual void drawCircle( const Coordinates& center, uint16_t radius, const Color& color ) {};
    virtual void drawHalfCircleLeft( const Coordinates& center, uint16_t radius, const Color& color ) {};
    virtual void drawHalfCircleRight( const Coordinates& center, uint16_t radius, const Color& color ) {};
    virtual void drawImage( const Coordinates& coords, const ImageMono& image, const Color& color ) {};
};

}  // namespace lcd
