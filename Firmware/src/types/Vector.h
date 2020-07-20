#pragma once

#include <cstdint>
#include <sys/_stdint.h>

struct Vector
{
    int16_t x;
    int16_t y;

    Vector( int16_t x_, int16_t y_ ) :
        x( x_ ),
        y( y_ )
    {
    }

    Vector& operator+=(const Vector& vector)
    {
        x = x + vector.x;
        y = y + vector.y;
        return *this;
    };
};
