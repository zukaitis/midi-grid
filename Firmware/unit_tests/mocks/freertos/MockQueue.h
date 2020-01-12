#pragma once

#include "freertos/queue.hpp"
#include "freertos/ticks.hpp"

#include <gmock/gmock.h>

namespace freertos
{

class MockQueue
{
public:
    static MockQueue& getInstance();

    MOCK_METHOD( bool, Enqueue, ( void* item ) );
    MOCK_METHOD( bool, Dequeue, ( void* item ) );
    MOCK_METHOD( void, Flush, () );

private:
    MockQueue() = default;
    virtual ~MockQueue() = default;
};

}
