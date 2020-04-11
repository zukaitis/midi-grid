#pragma once

#include "hardware/lcd/SpiInterface.h"

#include <cstdint>

namespace hardware
{

namespace lcd
{

class Spi: public SpiInterface
{
public:
    Spi();
    ~Spi() override;

    void initialize() const override;

    void reset() const override;
    void writeCommand( uint8_t command ) const override;
    void writeData( const uint8_t& data, const uint32_t size ) const override;
    void writeData( const etl::array_view<uint8_t>& data ) const override;

private:
    void initializeDma() const;
    void initializeGpio() const;
    void initializeSpi() const;
};

}  // namespace lcd

}  // namespace hardware
