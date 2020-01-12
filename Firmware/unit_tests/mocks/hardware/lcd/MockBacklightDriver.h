#pragma once

#include "hardware/lcd/BacklightDriverInterface.h"

#include <gmock/gmock.h>

namespace hardware
{

namespace lcd
{

class MockBacklightDriver : public BacklightDriverInterface
{
public:
    MOCK_METHOD( void, initialize, (), (override) );
    MOCK_METHOD( void, setIntensity, (const uint8_t intensity), (override) );

    MOCK_METHOD( uint8_t, getMaximumIntensity, (), (const, override) );
};

}

}
