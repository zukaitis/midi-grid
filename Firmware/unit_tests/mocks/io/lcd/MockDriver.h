#pragma once

#include "io/lcd/DriverInterface.h"

#include <gmock/gmock.h>

namespace lcd
{

class MockDriver : public DriverInterface
{
public:
    MOCK_METHOD( void, initialize, (), (override) );

    MOCK_METHOD( void, clear, (), (override) );
    MOCK_METHOD( void, clearArea, (const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2), (override) );
    MOCK_METHOD( void, displayImage, (const uint8_t x, const uint8_t y, const Image& image), (override) );
    MOCK_METHOD( void, putChar, (const uint8_t x, const uint8_t y, const char c), (override) );

    MOCK_METHOD( uint16_t, width, (), (const, override) );
    MOCK_METHOD( uint16_t, height, (), (const, override) );
    MOCK_METHOD( uint16_t, numberOfTextLines, (), (const, override) );
};

}
