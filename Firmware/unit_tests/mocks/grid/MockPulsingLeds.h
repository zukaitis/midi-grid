#pragma once
#include "grid/PulsingLedsInterface.h"

#include <gmock/gmock.h>

namespace grid
{

class MockPulsingLeds: public PulsingLedsInterface
{
public:

    MOCK_METHOD( void, add, ( const Coordinates& coordinates, const Color& color ), (override) );
    MOCK_METHOD( void, remove, ( const Coordinates& coordinates ), (override) );
    MOCK_METHOD( void, removeAll, (), (override) );
};

}
