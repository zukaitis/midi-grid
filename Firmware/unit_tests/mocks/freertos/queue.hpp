 #pragma once

#include "freertos/ticks.hpp"

namespace freertos
{

class Queue
{
public:
    Queue( uint32_t maxItems, uint32_t itemSize );
    Queue(); // exists only in mock class
    virtual ~Queue() = default;

    bool Enqueue( void* item );
    bool Dequeue( void* item, TickType_t Timeout = 0xFFFFFFFF );
    void Flush();
    bool IsFull();
};

}
