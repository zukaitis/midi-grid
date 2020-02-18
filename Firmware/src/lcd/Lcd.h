#pragma once

#include "lcd/LcdInterface.h"

namespace lcd
{

class DriverInterface;
class BacklightInterface;

class Lcd: public LcdInterface
{
public:
    Lcd( DriverInterface& driver, BacklightInterface& backlight );
    ~Lcd() override;

    void clear() override;
    void clearArea( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 ) override;
    void displayImage( uint8_t x, uint8_t y, const Image& image) override;
    void displayProgressArc( uint8_t x, uint8_t y, uint8_t position ) override;
    void initialize() override;
    void print( const char* string, uint8_t x, uint8_t y, Justification justification = Justification::LEFT ) override;
    // void print( const char* const string, const uint8_t y, const Justification justification ) override;
    void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y, Justification justification = Justification::LEFT ) override;
    void setBacklightIntensity( uint8_t intensity ) override;

    uint16_t right() const override;
    uint8_t maximumBacklightIntensity() const override;
    uint8_t horizontalCenter() const override // TODO(unknown): remove
    {
        return 84 / 2;
    };

private:

    void putString( const char* string, uint8_t x, uint8_t y );
    void putBigDigits( uint16_t number, uint8_t x, uint8_t y, uint8_t numberOfDigits );

    DriverInterface& driver_;
    BacklightInterface& backlight_;

};

}  // namespace lcd
