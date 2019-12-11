#pragma once

typedef uint32_t TickType_t;

namespace freertos
{

class Ticks
{
public:

    static TickType_t MsToTicks( const TickType_t milliseconds )
    {
        return milliseconds;
    }
};

}
