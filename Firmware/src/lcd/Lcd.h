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
    void print( const etl::string_view& string, uint8_t x, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t x, uint8_t y) override;
    void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y, Justification justification ) override;
    void printNumberInBigDigits( uint16_t number, uint8_t y, Justification justification ) override;
    void printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y ) override;
    void setBacklightIntensity( uint8_t intensity ) override;

    uint8_t maximumBacklightIntensity() const override;
    uint16_t line( uint8_t lineNumber ) const override;

    void release() override;

private:

    void putString( const etl::string_view& string, uint8_t x, uint8_t y );
    void putBigDigits( uint16_t number, uint8_t x, uint8_t y, uint8_t numberOfDigits );

    uint16_t calculateX( Justification justification );

    DriverInterface& driver_;
    BacklightInterface& backlight_;

};

}  // namespace lcd
