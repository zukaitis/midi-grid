#pragma once

#include "hardware/grid/Constants.h"

struct Coordinates;

namespace hardware
{
namespace grid
{

class OutputInterface
{
public:
    virtual void setRedOutput( const Coordinates& coords, const std::uint32_t value ) = 0;
    virtual void setGreenOutput( const Coordinates& coords, const std::uint32_t value ) = 0;
    virtual void setBlueOutput( const Coordinates& coords, const std::uint32_t value ) = 0;
    virtual void setAllOff() = 0;
};

}
}
