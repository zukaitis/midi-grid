#pragma once

#include "lcd/DriverInterface.h"
#include "lcd/image/ImageInterface.h"
#include "lcd/Parameters.h"
#include "types/Coordinates.h"

#include <cmath>
#include <etl/array.h>
#include <etl/vector.h>

namespace lcd
{

class ImageMono;

class Image : public ImageInterface
{
public:
    explicit Image( DriverInterface* driver );
    ~Image() override;

    void createNew( uint16_t width, uint16_t height ) override;
    void createNew( uint16_t width, uint16_t height, const Color& fill ) override;
    void createNew() override;
    void createNew( const Color& fill ) override;

    void clear() override;
    void fill( const Color& color ) override;

    void endDrawing() override;
    bool isBeingDrawn() override;
    void display( const Coordinates& coords ) override;
    void display() override;

    uint16_t width() override;
    uint16_t height() override;

    bool putPixel( const Coordinates& coords, uint8_t colorIndex ) override;
    uint8_t assignColorIndex( const Color& color ) override;
    uint8_t getBackgroundColorIndex() override;

private:
    void initialize( uint16_t width, uint16_t height );
    uint8_t getColorIndex( const Coordinates& pixel ) const;

    static const uint8_t colorBits_ = 3;
    static const uint8_t numberOfColors_ = 8 - 1;  // std::pow(2, colorBits_) - 1;
    static const uint8_t pixelsPerWord_ = 32 / colorBits_;
    static const uint8_t pixelMask_ = 8 - 1; // std::pow(2, colorBits_) - 1;
    static const uint16_t maxWidth_ = parameters::width;
    static const uint16_t maxHeight_ = parameters::height;
    static const uint16_t dataSize_ = maxWidth_ * (maxHeight_ / pixelsPerWord_);

    DriverInterface& driver_;

    uint16_t width_;
    uint16_t height_;
    uint16_t wordsPerColumn_;

    etl::vector<uint32_t, dataSize_> data_;
    etl::vector<Color, numberOfColors_> palette_;

    bool drawingInProgress_;
    uint8_t backgroundColorIndex_;
};

}  // namespace lcd
