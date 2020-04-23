#pragma once

#include "lcd/DriverInterface.h"
#include <cstdint>
#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>
#include <etl/array.h>

namespace hardware
{
    namespace lcd
    {
        class SpiInterface;
    }
}

namespace lcd
{

class Pcd8544: public DriverInterface, private freertos::Thread
{
public:
    Pcd8544( hardware::lcd::SpiInterface& spi );
    virtual ~Pcd8544();

    void initialize() override;

    void clear() override;
    void clearArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) override;
    void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) override;
    void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image ) override;
    
    void putString( const etl::string_view& string, const Coordinates& coords ) override;

    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;

private:
    void Run() override;

    void putChar( uint8_t x, uint8_t y, char c );
    void setCursor( uint8_t column, uint8_t row8Bit );
    void transmit();

    constexpr uint16_t width_ = 84;
    constexpr uint16_t height_ = 48;
    constexpr uint16_t numberOfTextLines_ = height_ / 8;

    hardware::lcd::SpiInterface& spi_;

    etl::array<etl::array<uint8_t, width_>, numberOfTextLines_> buffer_;
    freertos::BinarySemaphore updateRequired_;
};

}  // namespace lcd
