#pragma once
#include "io/grid/ButtonInputInterface.h"

#include <gmock/gmock.h>

namespace grid
{

class MockButtonInput: public ButtonInputInterface
{
public:
    MOCK_METHOD( bool, waitForEvent, ( ButtonEvent& event ), (override) );
    MOCK_METHOD( void, discardPendingEvents, (), (override) );
};

}
