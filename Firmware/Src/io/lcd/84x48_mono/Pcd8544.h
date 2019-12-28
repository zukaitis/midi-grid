#pragma once

#include "io/lcd/84x48_mono/Pcd8544Interface.h"
#include <cstdint>

namespace hardware
{
    namespace lcd
    {
        class DriverInterface;
    }
}

namespace lcd
{

class Pcd8544: public Pcd8544Interface
{
public:
    Pcd8544( hardware::lcd::DriverInterface& driver );
    virtual ~Pcd8544();

    void initialize() override;
    void transmit( const uint8_t& data, const uint32_t size ) override;

private:
    void setCursor( const uint8_t column, const uint8_t row8Bit );

    hardware::lcd::DriverInterface& driver_;
};

}
