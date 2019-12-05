#include "types/Color.h"

const uint8_t Color::maxValue = 64;

Color::Color( const uint8_t r, const uint8_t g, const uint8_t b ):
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

uint8_t Color::getRed() const
{
    return red_;
};

uint8_t Color::getGreen() const
{
    return green_;
};

uint8_t Color::getBlue() const
{
    return blue_;
};

bool Color::operator==(const Color& color) const
{
    bool equal = true;
    equal &= (this->red_ == color.red_);
    equal &= (this->green_ == color.green_);
    equal &= (this->blue_ == color.blue_);
    return equal;
};

Color Color::operator+(const Color& color) const
{
    const Color sum( this->red_ + color.red_, this->green_ + color.green_, this->blue_ + color.blue_ );
    return sum;
};

Color Color::operator*(const uint8_t multiplier) const
{
    const Color product( this->red_ * multiplier, this->green_ * multiplier, this->blue_ * multiplier );
    return product;
}

Color Color::operator*(const float multiplier) const
{
    const Color product( static_cast<uint8_t>(this->red_ * multiplier),
        static_cast<uint8_t>(this->green_ * multiplier),
        static_cast<uint8_t>(this->blue_ * multiplier) );
    return product;
}

Color Color::operator/(const uint8_t divisor) const
{
    const Color quotient( this->red_ / divisor, this->green_ / divisor, this->blue_ / divisor );
    return quotient;
}
