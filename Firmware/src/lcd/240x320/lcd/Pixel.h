#pragma once

#include "types/Color.h"

namespace lcd
{

class Pixel
{
public:
    Pixel() = default;
    explicit Pixel( Color color );

    bool operator==( const Pixel& other ) const;
    bool operator!=( const Pixel& other ) const;

private:
    uint16_t value_;
};

} // namespace lcd
