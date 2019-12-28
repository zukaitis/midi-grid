#pragma once

#include "hardware/lcd/DriverInterface.h"

#include <cstdint>

namespace hardware
{

namespace lcd
{

class Driver: public DriverInterface
{
public:
    Driver();
    virtual ~Driver();

    void initialize() const override;

    void reset() const override;
    void writeCommand( const uint8_t& command, const uint32_t size = 1 ) const override;
    void writeData( const uint8_t& data, const uint32_t size ) const override;

private:
    void initializeDma() const;
    void initializeGpio() const;
    void initializeSpi() const;
};

}

}