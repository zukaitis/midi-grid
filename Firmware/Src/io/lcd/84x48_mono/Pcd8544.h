#pragma once

#include "io/lcd/DriverInterface.h"
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
    void clearArea( const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2 ) override;
    void displayImage( const uint8_t x, const uint8_t y, const Image& image ) override;
    void putChar( const uint8_t x, const uint8_t y, const char c ) override;

    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;

private:
    void Run() override;

    void setCursor( const uint8_t column, const uint8_t row8Bit );
    void transmit();

    static constexpr uint16_t width_ = 84;
    static constexpr uint16_t height_ = 48;
    static constexpr uint16_t numberOfTextLines_ = height_ / 8;

    hardware::lcd::SpiInterface& spi_;

    etl::array<etl::array<uint8_t, width_>, numberOfTextLines_> buffer_;
    freertos::BinarySemaphore updateRequired_;
};

}
