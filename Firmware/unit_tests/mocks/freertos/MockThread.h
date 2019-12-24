#pragma once

#include "freertos/thread.hpp"
#include "freertos/ticks.hpp"

#include <gmock/gmock.h>

namespace freertos
{

class MockThread
{
public:
    static MockThread& getInstance();

    MOCK_METHOD( bool, Start, () );
    MOCK_METHOD( void, Suspend, () );
    MOCK_METHOD( void, Resume, () );
    MOCK_METHOD( void, DelayUntil, (const TickType_t delay) );

private:
    MockThread() = default;
    virtual ~MockThread() = default;
};

class ThreadCaller
{
public:
    static ThreadCaller& getInstance();

    void setThread( freertos::Thread& thread );

    void Run();

private:
    ThreadCaller() = default;
    ~ThreadCaller() = default;

    freertos::Thread* currentThread_;
};

}
