#pragma once

#include "types/Vector.h"
#include <cstdint>

struct Coordinates
{
    uint16_t x;
    uint16_t y;

    bool operator==(const Coordinates& coordinates) const
    {
        bool equal = true;
        equal &= (this->x == coordinates.x);
        equal &= (this->y == coordinates.y);
        return equal;
    };

    bool operator!=(const Coordinates& coordinates) const
    {
        return !(*this == coordinates);
    };

    bool operator<(const Coordinates& coordinates) const
    {
        bool lessThan = true;
        lessThan &= (this->x < coordinates.x);
        lessThan &= (this->y < coordinates.y);
        return lessThan;
    };

    bool operator<=(const Coordinates& coordinates) const
    {
        bool lessThanOrEqual = true;
        lessThanOrEqual &= (this->x <= coordinates.x);
        lessThanOrEqual &= (this->y <= coordinates.y);
        return lessThanOrEqual;
    };

    Vector operator-(const Coordinates& coordinates) const
    {
        const Vector difference = {static_cast<int16_t>(this->x - coordinates.x), static_cast<int16_t>(this->y - coordinates.y)};
        return difference;
    };

    Coordinates operator+(const Vector& vector) const
    {
        const Coordinates result = {static_cast<uint16_t>(this->x + vector.x), static_cast<uint16_t>(this->y + vector.y)};
        return result;
    };

    Coordinates operator-(const Vector& vector) const
    {
        const Coordinates result = {static_cast<uint16_t>(this->x - vector.x), static_cast<uint16_t>(this->y - vector.y)};
        return result;
    };
};
