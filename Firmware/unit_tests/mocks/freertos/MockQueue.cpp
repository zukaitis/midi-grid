#include "freertos/MockQueue.h"
#include "freertos/queue.hpp"

namespace freertos
{

MockQueue& MockQueue::getInstance()
{
    static MockQueue instance;
    return instance;
}

Queue::Queue( uint32_t maxItems, uint32_t itemSize )
{
}

Queue::Queue()
{
}

bool Queue::Enqueue( void* item )
{
    return MockQueue::getInstance().Enqueue( item );
}

bool Queue::Dequeue( void* item, TickType_t Timeout )
{
    return MockQueue::getInstance().Dequeue( item );
}

void Queue::Flush()
{
    MockQueue::getInstance().Flush();
}

bool Queue::IsFull()
{
    return MockQueue::getInstance().IsFull();
}

}
