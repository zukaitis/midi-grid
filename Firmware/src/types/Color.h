#pragma once

#include <cstdint>

class Color
{
public:

    Color() = default;
    Color( uint8_t r, uint8_t g, uint8_t b );

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
    static const uint8_t maxValue;

    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
};

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
