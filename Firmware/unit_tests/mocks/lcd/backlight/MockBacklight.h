#pragma once

#include "lcd/backlight/BacklightInterface.h"

#include <gmock/gmock.h>

namespace lcd
{

class MockBacklight : public BacklightInterface
{
public:
    MOCK_METHOD( void, initialize, (), (override) );
    MOCK_METHOD( void, setIntensity, (const uint8_t intensity), (override) );

    MOCK_METHOD( uint8_t, getMaximumIntensity, (), (const, override) );
};

}
