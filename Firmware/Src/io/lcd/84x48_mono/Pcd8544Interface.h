#pragma once

#include <cstdint>

namespace lcd
{

class Pcd8544Interface
{
public:
    virtual ~Pcd8544Interface() = default;

    virtual void initialize() = 0;
    virtual void transmit( const uint8_t& data, const uint32_t size ) = 0;
};

}
