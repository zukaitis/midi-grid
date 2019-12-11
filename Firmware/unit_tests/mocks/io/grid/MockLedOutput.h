#pragma once

#include "io/grid/LedOutputInterface.h"

#include <gmock/gmock.h>

namespace grid
{

class MockLedOutput : public LedOutputInterface
{
public:
    MOCK_METHOD( void, set, (const Coordinates& coordinates, const Color& color), (override) );
    MOCK_METHOD( void, setAllOff, (), (override) );
};

}
