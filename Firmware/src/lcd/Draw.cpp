#include "lcd/Draw.h"
#include "lcd/LcdInterface.h"
#include "types/Coordinates.h"

#include <algorithm>

namespace lcd
{

Draw::Draw( LcdInterface* lcd ):
    lcd_( *lcd ),
    image_(),
    imageData_()
{
}

Draw::~Draw() = default;

void Draw::line( const Coordinates& point1, const Coordinates& point2, const Pixel& color )
{
    const uint16_t width = std::abs( point1.x - point2.x ) + 1;
    const uint16_t height = std::abs( point1.y - point2.y ) + 1;
    const uint16_t bytesPerColumn = (height + 7) / 8;

    imageData_.assign( width * bytesPerColumn, 0U );
    image_ = Image( Image::DataView(imageData_), width, height );

    const Coordinates imageStart = { std::min( point1.x, point2.x ), std::min( point1.y, point2.y ) };
    putLine( {static_cast<uint16_t>(point1.x - imageStart.x), static_cast<uint16_t>(point1.y - imageStart.y)},
        {static_cast<uint16_t>(point2.x - imageStart.x), static_cast<uint16_t>(point2.y - imageStart.y)} );

    lcd_.displayImage( imageStart, image_, color );
}

void Draw::arc( const Coordinates& center, const uint16_t innerRadius, const uint16_t outerRadius,
    const uint16_t startAngle, const uint16_t endAngle, const Pixel& color )
{
    const uint16_t edgeLength = outerRadius * 2 - 1;
    const uint16_t bytesPerColumn = (edgeLength + 7) / 8;
    imageData_.assign( edgeLength * bytesPerColumn, 0U );
    image_ = Image( Image::DataView(imageData_), edgeLength, edgeLength );

    for (uint16_t radius = innerRadius; radius <= outerRadius; radius++)
    {
        uint16_t sectionStart = startAngle;
        while (sectionStart < endAngle)
        {
            const uint16_t ceiling = ((sectionStart / 45) + 1) * 45;
            const uint16_t sectionEnd = std::min( ceiling, endAngle );
            putArc( {outerRadius, outerRadius}, radius, sectionStart, sectionEnd );
            sectionStart = ceiling;
        }
    }

    const Coordinates imageStart = { static_cast<uint16_t>(center.x - outerRadius), static_cast<uint16_t>(center.y - outerRadius) };
    lcd_.displayImage( imageStart, image_, color );
}

void Draw::putLine( const Coordinates& point1, const Coordinates& point2 )
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
        putPixel( pnt );
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

void Draw::putArc( const Coordinates& center, const uint16_t radius, uint16_t startAngle, uint16_t endAngle )
{
    const uint16_t sectionIndex = (endAngle - 1) / 45;

    auto getCoordinates = [&]( uint16_t opp, uint16_t adj ) 
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
        Coordinates endPixel = {0, 0};

        if (0 != (sectionIndex % 2))  // counter-clockwise
        {
            std::swap( startAngle, endAngle );
        }

        if (0 != (startAngle % 45))
        {
            opposite = std::sin( startAngle % 45 ) * radius;
            adjacent = std::cos( startAngle % 45 ) * radius;
        }

        if (0 != (endAngle % 45))
        {
            endPixel = getCoordinates( std::sin( endAngle % 45 ) * radius, std::cos( endAngle % 45 ) * radius );
        }

        int err = 0;
        Coordinates pixel = {};
    
        do
        {
            pixel = getCoordinates( opposite, adjacent );
            putPixel( pixel );
        
            if (err <= 0)
            {
                opposite += 1;
                err += 2 * opposite + 1;
            }
        
            if (err > 0)
            {
                adjacent -= 1;
                err -= 2*adjacent + 1;
            }
        } while ((pixel != endPixel) && (adjacent >= opposite));
    }
}

void Draw::putPixel( const Coordinates& coords )
{
    const Coordinates imageLimits = {image_.getWidth(), image_.getHeight()};
    if (coords < imageLimits)
    {
        const uint16_t bytesPerColumn = (image_.getHeight() + 7) / 8;
        const uint16_t byteIndex = coords.x * bytesPerColumn + coords.y / 8;
        imageData_.at(byteIndex) |= 1U << (coords.y % 8U);
    }
}

}  // namespace lcd
