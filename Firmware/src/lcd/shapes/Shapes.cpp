#include "lcd/shapes/Shapes.h"

#include "types/Coordinates.h"
#include "types/Vector.h"
#include <sys/_stdint.h>

namespace lcd
{

constexpr auto M_PI = 3.14159265358979323846;

Shapes::Shapes( ImageInterface* image  ):
    image_( *image )
{
}

Shapes::~Shapes() = default;

void Shapes::drawLine( const Coordinates& point1, const Coordinates& point2, const Color& color )
{
    putLine( point1, point2, image_.assignColorIndex( color ) );
}

void Shapes::drawLine( const Coordinates& point1, const Coordinates& point2, const uint16_t thickness, const Color& color )
{
    const uint8_t colorIndex = image_.assignColorIndex( color );
    const uint16_t radius = (thickness - 1) / 2;

    putCircle( point1, radius, colorIndex );
    putCircle( point2, radius, colorIndex );

    const Vector line = point1 - point2;
    Vector delta0 = (std::abs( line.x ) > std::abs( line.y )) ? Vector( 0, 1 ) : Vector( 1, 0 );
    Vector delta = delta0;

    putLine( point1, point2, colorIndex );
    for (uint16_t i = 0; i < radius; i++)
    {
        putLine( point1 + delta, point2 + delta, colorIndex );
        putLine( point1 - delta, point2 - delta, colorIndex );

        delta += delta0;
    }
}

void Shapes::drawLine( const Coordinates& point1, const uint16_t angle, const uint16_t length,
    const uint16_t thickness, const Color& color )
{
    const Coordinates point2 = point1 +
        Vector( - std::sin( angle * M_PI / 180.0 ) * length, std::cos( angle * M_PI / 180.0 ) * length );
    drawLine( point1, point2, thickness, color );
}

void Shapes::drawArc( const Coordinates& center, const uint16_t innerRadius, const uint16_t outerRadius,
    const uint16_t startAngle, const uint16_t endAngle, const Color& color )
{
    const uint8_t colorIndex = image_.assignColorIndex( color );

    uint16_t sectionStart = startAngle;
    while (sectionStart < endAngle)
    {
        const uint16_t ceiling = ((sectionStart / 45) + 1) * 45;
        const uint16_t sectionEnd = std::min( ceiling, endAngle );
        putPie( center, outerRadius, sectionStart, sectionEnd, colorIndex );
        sectionStart = ceiling;
    }

    putCircle( center, innerRadius, image_.getBackgroundColorIndex(), CircleType::FULL );
}

void Shapes::drawCircle( const Coordinates& center, const uint16_t radius, const Color& color )
{
    putCircle( center, radius, image_.assignColorIndex( color ) );
}

void Shapes::drawHalfCircleLeft( const Coordinates& center, const uint16_t radius, const Color& color )
{
    putCircle( center, radius, image_.assignColorIndex( color ), CircleType::LEFT_HALF );
}

void Shapes::drawHalfCircleRight( const Coordinates& center, const uint16_t radius, const Color& color )
{
    putCircle( center, radius, image_.assignColorIndex( color ), CircleType::RIGHT_HALF );
}

void Shapes::drawImage( const Coordinates& coords, const ImageMono& image, const Color& color )
{
    const uint16_t width = image.getWidth();
    const uint16_t height = image.getHeight();
    const uint8_t bytesPerColumn = (height + 7) / 8;
    const uint8_t colorIndex = image_.assignColorIndex( color );

    for (uint16_t x = 0; x < width; x++)
    {
        for (uint16_t y = 0; y < height; y++)
        {
            const uint8_t byte = image.getData().at( x * bytesPerColumn + y / 8U );
            const bool pixelActive = ((static_cast<uint8_t>(byte >> (y % 8U)) & 0x01U) != 0U);
            if (pixelActive)
            {
                image_.putPixel( {static_cast<uint16_t>(coords.x + x), static_cast<uint16_t>(coords.y + y)}, colorIndex );
            }
        }
    }
}

void Shapes::putLine( const Coordinates& point1, const Coordinates& point2, const uint8_t colorIndex )
{
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    const int16_t dx = std::abs( point1.x - point2.x );
    const int16_t sx = (point1.x < point2.x) ? 1 : -1;
    const int16_t dy = - std::abs( point1.y - point2.y );
    const int16_t sy = (point1.y < point2.y) ? 1 : -1;
    int16_t err = dx + dy;  /* error value e_xy */

    Coordinates pnt = point1;
    while (true)
    {
        image_.putPixel( pnt, colorIndex );

        if (pnt == point2)
        {
            break;
        }
        const int16_t e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy; /* e_xy+e_x > 0 */
            pnt.x += sx;
        }
        if (e2 <= dx) /* e_xy+e_y < 0 */
        {
            err += dx;
            pnt.y += sy;
        }
    }
}

void Shapes::putCircle( const Coordinates& center, const uint16_t radius, const uint8_t colorIndex, const CircleType type )
{
    uint16_t opposite = 0;
    uint16_t adjacent = radius;
    int16_t err = 0;

    while (opposite <= adjacent)
    {
        if ((CircleType::LEFT_HALF == type) || (CircleType::FULL == type))
        {
            putLine( {static_cast<uint16_t>(center.x - adjacent), static_cast<uint16_t>(center.y + opposite)},
                {static_cast<uint16_t>(center.x - adjacent), static_cast<uint16_t>(center.y - opposite)}, colorIndex );
            putLine( {static_cast<uint16_t>(center.x - opposite), static_cast<uint16_t>(center.y + adjacent)},
                {static_cast<uint16_t>(center.x - opposite),static_cast<uint16_t>(center.y - adjacent)}, colorIndex );
        }
        
        if ((CircleType::RIGHT_HALF == type) || (CircleType::FULL == type))
        {
            putLine( {static_cast<uint16_t>(center.x + adjacent), static_cast<uint16_t>(center.y + opposite)},
                {static_cast<uint16_t>(center.x + adjacent), static_cast<uint16_t>(center.y - opposite)}, colorIndex );
            putLine( {static_cast<uint16_t>(center.x + opposite), static_cast<uint16_t>(center.y + adjacent)},
                {static_cast<uint16_t>(center.x + opposite),static_cast<uint16_t>(center.y - adjacent)}, colorIndex );
        }
    
        if (err <= 0)
        {
            opposite++;
            err += 2 * opposite + 1;
        }
    
        if (err > 0)
        {
            adjacent--;
            err -= 2 * adjacent + 1;
        }
    }
}

void Shapes::putPie( const Coordinates& center, const uint16_t radius, uint16_t startAngle, uint16_t endAngle, uint8_t colorIndex )
{
    const uint16_t sectionIndex = (endAngle - 1) / 45;

    auto getCoordinates = [&](const uint16_t opp, const uint16_t adj)
    {
        Coordinates coords = {};
        switch (sectionIndex)
        {
            case 0:
                coords = {static_cast<uint16_t>(center.x - opp), static_cast<uint16_t>(center.y + adj)};
                break;
            case 1:
                coords = {static_cast<uint16_t>(center.x - adj), static_cast<uint16_t>(center.y + opp)};
                break;
            case 2:
                coords = {static_cast<uint16_t>(center.x - adj), static_cast<uint16_t>(center.y - opp)};
                break;
            case 3:
                coords = {static_cast<uint16_t>(center.x - opp), static_cast<uint16_t>(center.y - adj)};
                break;
            case 4:
                coords = {static_cast<uint16_t>(center.x + opp), static_cast<uint16_t>(center.y - adj)};
                break;
            case 5:
                coords = {static_cast<uint16_t>(center.x + adj), static_cast<uint16_t>(center.y - opp)};
                break;
            case 6:
                coords = {static_cast<uint16_t>(center.x + adj), static_cast<uint16_t>(center.y + opp)};
                break;
            case 7:
            default:
                coords = {static_cast<uint16_t>(center.x + opp), static_cast<uint16_t>(center.y + adj)};
                break;
        }
        return coords;
    };

    if ((endAngle - startAngle) <= 45)
    {
        uint16_t opposite = 0;
        uint16_t adjacent = radius;
        volatile uint16_t oppositeLimitLow = 0;
        uint16_t oppositeLimitHigh = radius;

        if (0 != (sectionIndex % 2))  // counter-clockwise
        {
            std::swap( startAngle, endAngle );
            startAngle = 45 - (startAngle % 45);
            endAngle = 45 - (endAngle % 45);
        }

        if (0 != (startAngle % 45))
        {
            oppositeLimitLow = std::sin( (startAngle % 45) * M_PI / 180.0 ) * radius;
        }

        if (0 != (endAngle % 45))
        {
            oppositeLimitHigh = std::sin( (endAngle % 45) * M_PI / 180.0 ) * radius;
        }

        int16_t err = 0;

        while ((opposite <= adjacent) && (opposite <= oppositeLimitHigh))
        {
            if (opposite >= oppositeLimitLow)
            {
                putLine( center, getCoordinates( opposite, adjacent ), colorIndex );
                if ((opposite > 7) && (opposite < oppositeLimitHigh))
                {
                    // only draw additional lines, when they are necessary to ensure that no pixels stay unset
                    putLine( getCoordinates( 0, 1 ), getCoordinates( opposite - 1, adjacent ), colorIndex );
                    putLine( getCoordinates( 1, 0 ), getCoordinates( opposite, adjacent - 1 ), colorIndex );
                }
            }
        
            if (err <= 0)
            {
                opposite++;
                err += 2 * opposite + 1;
            }
        
            if (err > 0)
            {
                adjacent--;
                err -= 2 * adjacent + 1;
            }
        }
    }
}

}  // namespace lcd
