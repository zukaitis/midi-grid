#pragma once

#include "lcd/DrawInterface.h"
#include "lcd/LcdInterface.h"
#include "lcd/Image.h"
#include "types/Coordinates.h"

#include <etl/vector.h>

namespace lcd
{

class Draw : public DrawInterface
{
public:
    Draw( LcdInterface* lcd );
    ~Draw() override;

    void line( const Coordinates& point1, const Coordinates& point2, const Pixel& color ) override;
    void arc( const Coordinates& center, uint16_t innerRadius, uint16_t outerRadius,
        uint16_t startAngle, uint16_t endAngle, const Pixel& color ) override;

private:
    void putLine( const Coordinates& point1, const Coordinates& point2 );
    void putPie( const Coordinates& center, uint16_t radius, uint16_t startAngle, uint16_t endAngle );
    void putPixel( const Coordinates& coords );

    LcdInterface& lcd_;

    Image image_;
    etl::vector<uint8_t, 9600> imageData_;
};

}  // namespace lcd
