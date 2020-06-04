#pragma once

#include "lcd/DriverInterface.h"
#include "hardware/lcd/SpiInterface.h"
#include <cstdint>
#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>
#include <etl/array.h>
#include <etl/vector.h>
#include <etl/array_view.h>

namespace lcd
{

class Ili9341: public DriverInterface
{
public:
    explicit Ili9341( hardware::lcd::SpiInterface* spi );
    virtual ~Ili9341();

    void initialize() override;

    void fill( const Color& color ) override;
    void fillArea( const Coordinates& corner1, const Coordinates& corner2, const Color& color ) override;

    void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image ) override;
    void putImage( const Coordinates& coords, const ImageMono& image, const ImageColors& colors ) override;
    uint16_t putString( const etl::string_view& string, const Coordinates& coords ) override;
    uint16_t putString( const etl::string_view& string, const Coordinates& coords, const Format& format ) override;

    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;

private:
    static constexpr uint16_t width_ = 240U;
    static constexpr uint16_t height_ = 320U;
    static constexpr uint16_t numberOfTextLines_ = height_ / 8;

    using PixelBuffer = etl::vector<Color, width_*2>;
    using DataBuffer = etl::vector<uint8_t, 16>;

    void setWorkingArea( const Coordinates& topLeft, const Coordinates& bottomRight );
    static void fillPixelBuffer( PixelBuffer* buffer, const ImageMono& image, const ImageColors& colors,
        uint16_t firstLine, uint16_t lastLine );
    void appendToImageData( const etl::array_view<const uint8_t>& source );

    hardware::lcd::SpiInterface& spi_;

    etl::array<PixelBuffer, 2> pixelBuffer_;
    uint8_t pixelBufferIndex_;
    etl::array<DataBuffer, 2> dataBuffer_;
    uint8_t dataBufferIndex_;
    etl::vector<uint8_t, 9600> imageData_;
    etl::vector<uint8_t, 160> zeros_;

    PixelBuffer& assignPixelBuffer();
    DataBuffer& assignDataBuffer();

};

}