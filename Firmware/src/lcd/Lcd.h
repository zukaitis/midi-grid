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

    uint16_t right() const override;
    uint8_t maximumBacklightIntensity() const override;
    uint8_t horizontalCenter() const override // TODO: remove
    {
        return 84 / 2;
    };

private:

    void putString( const char* string, uint8_t x, const uint8_t y );
    void putBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits );

    DriverInterface& driver_;
    BacklightInterface& backlight_;

};

}
