#pragma once
#include "system/GlobalInterruptsInterface.h"

#include <gmock/gmock.h>

namespace mcu
{

class MockGlobalInterrupts: public GlobalInterruptsInterface
{
public:
    MOCK_METHOD( void, enable, (), (override) );
    MOCK_METHOD( void, disable, (), (override) );
};

}