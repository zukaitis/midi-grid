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

class Ili9341: public DriverInterface
{
public:
    explicit Ili9341( hardware::lcd::SpiInterface* spi );
    virtual ~Ili9341();

    void initialize() override;

    void clear() override;
    void clearArea( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 ) override;
    void displayImage( uint8_t x, uint8_t y, const Image& image ) override;
    void putChar( uint8_t x, uint8_t y, char c ) override;

    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;

private:

    void setCursor( const uint8_t column, const uint8_t row8Bit );
    void transmit();

    static constexpr uint16_t width_ = 240;
    static constexpr uint16_t height_ = 320;
    static constexpr uint16_t numberOfTextLines_ = height_ / 8;

    hardware::lcd::SpiInterface& spi_;
};

}