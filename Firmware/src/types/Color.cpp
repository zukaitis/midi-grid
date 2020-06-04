#include "types/Color.h"

const uint8_t Color::maxValue = 255;

Color::Color( const uint8_t r, const uint8_t g, const uint8_t b ):
    red_( r ),
    green_( g ),
    blue_( b )
{
}

uint8_t Color::red() const
{
    return red_;
};

uint8_t Color::green() const
{
    return green_;
};

uint8_t Color::blue() const
{
    return blue_;
};

bool Color::operator==(const Color& other) const
{
    bool equal = true;
    equal &= (this->red_ == other.red_);
    equal &= (this->green_ == other.green_);
    equal &= (this->blue_ == other.blue_);
    return equal;
};

bool Color::operator!=(const Color& other) const
{
    return !(other == *this);
};

Color Color::operator+(const Color& addend) const
{
    const uint16_t sumRed = this->red_ + addend.red_;
    const uint16_t sumGreen = this->green_ + addend.green_;
    const uint16_t sumBlue = this->blue_ + addend.blue_;

    const Color sum( (sumRed > maxValue) ? maxValue : sumRed,
        (sumGreen > maxValue) ? maxValue : sumGreen,
        (sumBlue > maxValue) ? maxValue : sumBlue );
    return sum;
};

Color Color::operator*(const uint8_t multiplier) const
{
    const uint16_t productRed = this->red_ * multiplier;
    const uint16_t productGreen = this->green_ * multiplier;
    const uint16_t productBlue = this->blue_ * multiplier;

    const Color product( (productRed > maxValue) ? maxValue : productRed,
        (productGreen > maxValue) ? maxValue : productGreen,
        (productBlue > maxValue) ? maxValue : productBlue );
    return product;
}

Color Color::operator*(const float multiplier) const
{
    const uint16_t productRed = this->red_ * multiplier;
    const uint16_t productGreen = this->green_ * multiplier;
    const uint16_t productBlue = this->blue_ * multiplier;

    const Color product( (productRed > maxValue) ? maxValue : productRed,
        (productGreen > maxValue) ? maxValue : productGreen,
        (productBlue > maxValue) ? maxValue : productBlue );
    return product;
}

Color Color::operator/(const uint8_t divisor) const
{
    const uint16_t quotientRed = this->red_ / divisor;
    const uint16_t quotientGreen = this->green_ / divisor;
    const uint16_t quotientBlue = this->blue_ / divisor;

    const Color quotient( (quotientRed > maxValue) ? maxValue : quotientRed,
        (quotientGreen > maxValue) ? maxValue : quotientGreen,
        (quotientBlue > maxValue) ? maxValue : quotientBlue );
    return quotient;
}
