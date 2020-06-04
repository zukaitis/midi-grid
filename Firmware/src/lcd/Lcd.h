#pragma once

#include "lcd/LcdInterface.h"
#include "lcd/Draw.h"

namespace lcd
{

class DriverInterface;
class BacklightInterface;

class Lcd: public LcdInterface
{
public:
    Lcd( DriverInterface& driver, BacklightInterface& backlight );
    ~Lcd() override;

    void setBackgroundColor( const Color& color ) override;
    void clear() override;
    void clearArea( const Coordinates& corner1, const Coordinates& corner2 ) override;
    void fillArea( const Coordinates& corner1, const Coordinates& corner2, const Color& color ) override;

    void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image) override;
    void displayImage( const Coordinates& coords, const ImageMono& image, const Color& color ) override;
    void displayProgressArc( uint8_t x, uint8_t y, uint8_t position ) override;
    void initialize() override;
    void print( const etl::string_view& string, uint8_t x, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t x, uint8_t y) override;
    uint16_t print( const etl::string_view& string, const Coordinates& coords, const Format& format ) override;
    void print( const etl::string_view& string, uint8_t y, const Format& format ) override;
    void setBacklightIntensity( uint8_t intensity ) override;

    uint8_t maximumBacklightIntensity() const override;
    uint16_t line( uint8_t lineNumber ) const override;

    void release() override;

    DrawInterface& draw() override;

private:

    uint16_t calculateX( Justification justification );

    Draw draw_;
    DriverInterface& driver_;
    BacklightInterface& backlight_;

    Color backgroundColor_;
};

}  // namespace lcd
