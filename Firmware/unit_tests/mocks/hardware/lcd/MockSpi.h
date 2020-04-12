#pragma once

#include "hardware/lcd/SpiInterface.h"

#include <gmock/gmock.h>

namespace hardware
{

namespace lcd
{

class MockSpi : public SpiInterface
{
public:
    MOCK_METHOD( void, initialize, (), (const, override) );

    MOCK_METHOD( void, reset, (), (const, override) );
    MOCK_METHOD( void, writeCommand, (const uint8_t command), (const, override) );
    MOCK_METHOD( void, writeData, (const uint8_t& data, uint32_t size), (const, override) );
    MOCK_METHOD( void, writeData, (const ::lcd::RawDataView& data), (const, override) );
    MOCK_METHOD( void, writeData, (const ::lcd::PixelView& data), (const, override) );
};

}

}
