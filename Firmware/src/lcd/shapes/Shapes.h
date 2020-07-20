#pragma once

#include "lcd/shapes/ShapesInterface.h"
#include "lcd/LcdInterface.h"
#include "lcd/ImageMono.h"
#include "types/Coordinates.h"

#include <etl/vector.h>

namespace lcd
{

class Shapes : public ShapesInterface
{
public:
    explicit Shapes( ImageInterface* image );
    ~Shapes() override;

    void drawLine( const Coordinates& point1, const Coordinates& point2, const Color& color ) override;
    void drawLine( const Coordinates& point1, const Coordinates& point2, uint16_t thickness, const Color& color ) override;
    void drawLine( const Coordinates& point1, uint16_t angle, uint16_t length, uint16_t thickness, const Color& color ) override;
    void drawArc( const Coordinates& center, uint16_t innerRadius, uint16_t outerRadius,
        uint16_t startAngle, uint16_t endAngle, const Color& color ) override;
    void drawCircle( const Coordinates& center, uint16_t radius, const Color& color ) override;
    void drawHalfCircleLeft( const Coordinates& center, uint16_t radius, const Color& color ) override;
    void drawHalfCircleRight( const Coordinates& center, uint16_t radius, const Color& color ) override;
    void drawImage( const Coordinates& coords, const ImageMono& image, const Color& color ) override;

private:
    enum class CircleType : uint8_t
    {
        FULL = 0,
        LEFT_HALF,
        RIGHT_HALF
    };

    void putLine( const Coordinates& point1, const Coordinates& point2, uint8_t colorIndex );
    void putCircle( const Coordinates& center, uint16_t radius, uint8_t colorIndex, CircleType type = CircleType::FULL );
    void putPie( const Coordinates& center, uint16_t radius, uint16_t startAngle, uint16_t endAngle, uint8_t colorIndex );

    ImageInterface& image_;
};

}  // namespace lcd
