#pragma once

#include <cstdint>
#include <etl/array_view.h>
#include <sys/_stdint.h>

class Color
{
public:

    Color() = default;
    Color( uint8_t r, uint8_t g, uint8_t b );
    explicit Color( uint32_t hexValue );

    uint8_t red() const;
    uint8_t green() const;
    uint8_t blue() const;

    bool operator==( const Color& other ) const;
    bool operator!=( const Color& other ) const;
    Color operator+( const Color& addend ) const;
    Color operator*( uint8_t multiplier ) const;
    Color operator*( float multiplier ) const;
    Color operator/( uint8_t divisor ) const;

private:
    static const uint8_t maxValue_;

    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
};

using ColorView = etl::array_view<const Color>;

namespace color
{
    const Color RED( 255, 0, 0 );
    const Color GREEN( 0, 255, 0 );
    const Color BLUE( 0, 0, 255 );
    const Color YELLOW = RED + GREEN;
    const Color MAGENTA = RED + BLUE;
    const Color CYAN = GREEN + BLUE;
    const Color WHITE = RED + GREEN + BLUE;
    const Color ORANGE( 255, 127, 0 );
    const Color SPRING_GREEN( 127, 255, 0 );
    const Color TURQUOISE( 0, 255, 127 );
    const Color OCEAN( 0, 127, 255 );
    const Color VIOLET( 127, 0, 255 );
    const Color RASPBERRY( 255, 0, 127 );
    const Color BLACK( 0, 0, 0 );
}  // namespace color
