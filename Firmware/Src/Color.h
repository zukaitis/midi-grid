#pragma once

#include <stdint.h>

class Color
{
public:

    Color():
        red_( 0 ),
        green_( 0 ),
        blue_( 0 )
    {
    };

    Color( const uint8_t r, const uint8_t g, const uint8_t b ):
        red_( r ),
        green_( g ),
        blue_( b )
    {
        if (red_ > maxValue)
        {
            red_ = maxValue;
        }

        if (green_ > maxValue)
        {
            green_ = maxValue;
        }

        if (blue_ > maxValue)
        {
            blue_ = maxValue;
        }
    }

    uint8_t getRed() const
    {
        return red_;
    };

    uint8_t getGreen() const
    {
        return green_;
    };

    uint8_t getBlue() const
    {
        return blue_;
    };

    bool operator==(const Color& color) const
    {
        bool equal = true;
        equal &= (this->red_ == color.red_);
        equal &= (this->green_ == color.green_);
        equal &= (this->blue_ == color.blue_);
        return equal;
    };

    Color operator+(const Color& color) const
    {
        const Color sum( this->red_ + color.red_, this->green_ + color.green_, this->blue_ + color.blue_ );
        return sum;
    };

    Color operator*(const uint8_t multiplier) const
    {
        const Color product( this->red_ * multiplier, this->green_ * multiplier, this->blue_ * multiplier );
        return product;
    }

    Color operator/(const uint8_t divisor) const
    {
        const Color quotient( this->red_ / divisor, this->green_ / divisor, this->blue_ / divisor );
        return quotient;
    }

private:
    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;

    static const uint8_t maxValue = 64;
};

namespace color
{
    const Color RED( 64, 0, 0 );
    const Color GREEN( 0, 64, 0 );
    const Color BLUE( 0, 0, 64 );
    const Color YELLOW = RED + GREEN;
    const Color MAGENTA = RED + BLUE;
    const Color CYAN = GREEN + BLUE;
    const Color WHITE = RED + GREEN + BLUE;
    const Color ORANGE( 64, 32, 0 );
    const Color SPRING_GREEN( 32, 64, 0 );
    const Color TURQUOISE( 0, 64, 32 );
    const Color OCEAN( 0, 32, 64 );
    const Color VIOLET( 32, 0, 64 );
    const Color RASPBERRY( 64, 0, 32 );
}
