#pragma once

struct Color;
struct Coordinates;

namespace grid
{

class PulsingLedsInterface
{
public:
    virtual ~PulsingLedsInterface() = default;

    virtual void add( const Coordinates& coordinates, const Color& color ) = 0;
    virtual void remove( const Coordinates& coordinates ) = 0;
    virtual void removeAll() = 0;
};

}
