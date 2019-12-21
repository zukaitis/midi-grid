#pragma once
#include "io/grid/FlashingLedsInterface.h"

#include <gmock/gmock.h>

namespace grid
{

class MockFlashingLeds: public FlashingLedsInterface
{
public:

    MOCK_METHOD( void, add, ( const Coordinates& coordinates, const FlashingColors& colors ), (override) );
    MOCK_METHOD( void, remove, ( const Coordinates& coordinates ), (override) );
    MOCK_METHOD( void, removeAll, (), (override) );
};

}
