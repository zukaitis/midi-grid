 #pragma once

#include "freertos/ticks.hpp"

#include <gmock/gmock.h>

namespace freertos
{

class Thread
{
public:
    Thread( const char* const name, const uint16_t stackDepth, const uint8_t priority ) {};
    virtual ~Thread() {};
    virtual void Run() = 0;

    MOCK_METHOD( void, Start, () );
    MOCK_METHOD( void, Suspend, () );
    MOCK_METHOD( void, Resume, () );
    MOCK_METHOD( void, DelayUntil, (const TickType_t delay) );
};

}
