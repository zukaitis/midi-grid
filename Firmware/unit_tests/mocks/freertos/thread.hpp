 #pragma once

#include "freertos/ticks.hpp"

namespace freertos
{

class Thread
{
public:
    Thread( const char* Name, uint16_t StackDepth, uint32_t Priority );
    Thread() = default;
    virtual ~Thread() = default;
    virtual void Run() = 0;

    bool Start();
    void Suspend();
    void Resume();
    void DelayUntil( const TickType_t delay );
};

}
