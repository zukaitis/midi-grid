#pragma once

#include <cstdint>
#include <etl/exception.h>

struct Coordinates
{
    uint8_t x;
    uint8_t y;

    bool operator==(const Coordinates& coordinates) const
    {
        bool equal = true;
        equal &= (this->x == coordinates.x);
        equal &= (this->y == coordinates.y);
        return equal;
    };

    bool operator<(const Coordinates& coordinates) const
    {
        bool equal = true;
        equal &= (this->x < coordinates.x);
        equal &= (this->y < coordinates.y);
        return equal;
    };
};
