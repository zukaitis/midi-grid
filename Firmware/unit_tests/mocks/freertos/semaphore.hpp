#pragma once

#include "freertos/ticks.hpp"

namespace freertos
{

class BinarySemaphore
{
public:
    BinarySemaphore( bool set = false );
    virtual ~BinarySemaphore() = default;

    bool Take( TickType_t Timeout = 0xFFFFFFFF );
    bool Give();
};

}
