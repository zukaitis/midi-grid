#pragma once

#include "types/Coordinates.h"

namespace hardware
{
namespace grid
{

class TestingInterface
{
public:
    virtual ~TestingInterface() = default;
    virtual void injectButtonPress( const Coordinates& coords ) = 0;
};

}  // namespace grid
}  // namespace hardware
