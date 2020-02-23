#pragma once

#include "hardware/grid/Constants.h"
#include "types/Coordinates.h"

namespace testing
{

class TestingInterface
{
public:
    virtual ~TestingInterface() = default;
    virtual void injectButtonPress( const Coordinates& coords ) = 0;
};

}  // namespace testing
