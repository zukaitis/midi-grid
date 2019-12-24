 #pragma once

#include "freertos/ticks.hpp"

class ThreadCaller;

namespace freertos
{

class Thread
{
public:
    friend class ThreadCaller;

    Thread( const char* Name, uint16_t StackDepth, uint32_t Priority );
    Thread(); // exists only in mock class
    virtual ~Thread() = default;
    virtual void Run() = 0;

    bool Start();
    void Suspend();
    void Resume();
    void DelayUntil( const TickType_t delay );
    uint32_t WaitForNotification( TickType_t Timeout = 0xFFFFFFFF );
};

}
