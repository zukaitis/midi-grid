#pragma once

struct Coordinates;
struct Color;

namespace grid
{

class LedOutputInterface
{
public:
    virtual ~LedOutputInterface() {};
    virtual void set( const Coordinates& coordinates, const Color& color ) = 0;
    virtual void setAllOff() = 0;
};

}
