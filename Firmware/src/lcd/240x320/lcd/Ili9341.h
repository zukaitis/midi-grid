#pragma once

#include "lcd/DriverInterface.h"
#include "lcd/Parameters.h"
#include "lcd/Pixel.h"
#include "hardware/lcd/SpiInterface.h"
#include "types/Coordinates.h"
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
    ~Ili9341() override;

    void initialize() override;

    void fill( const Color& color ) override;

    void putPixel( const Coordinates& coords, const Color& color ) override;
    void flush() override;

private:
    void refresh();
    void refreshInterlaced();

    void updateRefreshArea( const Coordinates& coords );
    void resetRefreshArea();

    void setWorkingArea( const Coordinates& topLeft, const Coordinates& bottomRight );
    void setWorkingLimitsX( uint16_t left, uint16_t right );
    void setWorkingLimitsY( uint16_t top, uint16_t bottom );

    hardware::lcd::SpiInterface& spi_;

    using DataBuffer = etl::vector<uint8_t, 16>;
    etl::array<DataBuffer, 2> dataBuffer_;
    uint8_t dataBufferIndex_;

    etl::array<etl::array<Pixel, parameters::width>, parameters::height> buffer_;
    Coordinates refreshAreaTopLeft_;
    Coordinates refreshAreaBottomRight_;

    DataBuffer& assignDataBuffer();
};

}  // namespace lcd
