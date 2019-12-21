#include "freertos/MockThread.h"
#include "freertos/thread.hpp"

namespace freertos
{

MockThread& MockThread::getInstance()
{
    static MockThread instance;
    return instance;
}

MockThread::MockThread()
{
}

MockThread::~MockThread()
{
}

Thread::Thread( const char* Name, uint16_t StackDepth, uint32_t Priority )
{
}

Thread::Thread()
{
}

Thread::~Thread()
{
}

bool Thread::Start()
{
    return MockThread::getInstance().Start();
}

void Thread::Suspend()
{
    MockThread::getInstance().Suspend();
}

void Thread::Resume()
{
    MockThread::getInstance().Resume();
}

void Thread::DelayUntil( const TickType_t delay )
{
    MockThread::getInstance().DelayUntil( delay );
}

}
