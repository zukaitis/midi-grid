#pragma once

#include "io/lcd/LcdInterface.h"

#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>
#include <etl/array.h>
#include <etl/cstring.h>

namespace lcd
{

class Pcd8544Interface;
class BacklightInterface;

class Lcd: private freertos::Thread, public LcdInterface
{
public:
    Lcd( Pcd8544Interface& driver, BacklightInterface& backlight );
    virtual ~Lcd();

    void clear() override;
    void clearArea( const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2 ) override;
    void displayImage( const uint8_t x, const uint8_t y, const Image image) override;
    void displayProgressArc( const uint8_t x, const uint8_t y, const uint8_t position ) override;
    void initialize() override;
    void print( const char* const string, const uint8_t x, const uint8_t y, const Justification justification = Justification::LEFT ) override;
    // void print( const char* const string, const uint8_t y, const Justification justification ) override;
    void printNumberInBigDigits( const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification = Justification::LEFT ) override;
    void setBacklightIntensity( const uint8_t intensity ) override;
    
    uint16_t width() const override;
    uint16_t height() const override;
    uint16_t numberOfTextLines() const override;
    uint8_t maximumBacklightIntensity() const override;

    uint8_t horizontalCenter() const override // TODO: remove
    {
        return width_ / 2;
    };

private:
    void Run() override;

    void putChar( const uint8_t x, const uint8_t y, const char c );
    void putString( const char* string, uint8_t x, const uint8_t y );
    void putBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits );

    Pcd8544Interface& driver_;
    BacklightInterface& backlight_;

    static constexpr uint16_t width_ = 84;
    static constexpr uint16_t height_ = 48;
    static constexpr uint16_t numberOfTextLines_ = height_ / 8;

    etl::array<etl::array<uint8_t, width_>, numberOfTextLines_> buffer_;

    freertos::BinarySemaphore updateRequired_;
};

}
