#include "types/Color.h"

const uint8_t Color::maxValue_ = 255;

Color::Color():
    red_( 0 ),
    green_( 0 ),
    blue_( 0 )
{
}

Color::Color( const uint8_t r, const uint8_t g, const uint8_t b ):
    red_( r ),
    green_( g ),
    blue_( b )
{
}

Color::Color( const uint32_t hexValue ):
    red_( (hexValue & 0xFF0000U) >> 16U ),
    green_( (hexValue & 0xFF00U) >> 8U ),
    blue_( hexValue & 0xFFU )
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

    const Color sum( (sumRed > maxValue_) ? maxValue_ : sumRed,
        (sumGreen > maxValue_) ? maxValue_ : sumGreen,
        (sumBlue > maxValue_) ? maxValue_ : sumBlue );
    return sum;
};

Color Color::operator*(const uint8_t multiplier) const
{
    const uint16_t productRed = this->red_ * multiplier;
    const uint16_t productGreen = this->green_ * multiplier;
    const uint16_t productBlue = this->blue_ * multiplier;

    const Color product( (productRed > maxValue_) ? maxValue_ : productRed,
        (productGreen > maxValue_) ? maxValue_ : productGreen,
        (productBlue > maxValue_) ? maxValue_ : productBlue );
    return product;
}

Color Color::operator*(const float multiplier) const
{
    const uint16_t productRed = this->red_ * multiplier;
    const uint16_t productGreen = this->green_ * multiplier;
    const uint16_t productBlue = this->blue_ * multiplier;

    const Color product( (productRed > maxValue_) ? maxValue_ : productRed,
        (productGreen > maxValue_) ? maxValue_ : productGreen,
        (productBlue > maxValue_) ? maxValue_ : productBlue );
    return product;
}

Color Color::operator/(const uint8_t divisor) const
{
    const uint16_t quotientRed = this->red_ / divisor;
    const uint16_t quotientGreen = this->green_ / divisor;
    const uint16_t quotientBlue = this->blue_ / divisor;

    const Color quotient( (quotientRed > maxValue_) ? maxValue_ : quotientRed,
        (quotientGreen > maxValue_) ? maxValue_ : quotientGreen,
        (quotientBlue > maxValue_) ? maxValue_ : quotientBlue );
    return quotient;
}
