#pragma once

#include "lcd/DriverInterface.h"
#include "hardware/lcd/SpiInterface.h"
#include <cstdint>
#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>
#include <etl/array.h>
#include <etl/vector.h>

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
    void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) override;
    void displayImage( uint8_t x, uint8_t y, const Image& image ) override;
    void putString( const etl::string_view& string, const Coordinates& coords ) override;

    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;

private:

    void setWorkingArea( const Coordinates& topLeft, const Coordinates& bottomRight );

    hardware::lcd::SpiInterface& spi_;

    static constexpr uint16_t width_ = 240U;
    static constexpr uint16_t height_ = 320U;
    static constexpr uint16_t numberOfTextLines_ = height_ / 8;

    using PixelBuffer = etl::vector<Pixel, height_>;
    using DataBuffer = etl::vector<uint8_t, 16>;

    etl::array<PixelBuffer, 2> pixelBuffer_;
    uint8_t pixelBufferIndex_;
    etl::array<DataBuffer, 2> dataBuffer_;
    uint8_t dataBufferIndex_;

    PixelBuffer& assignPixelBuffer();
    DataBuffer& assignDataBuffer();

};

}