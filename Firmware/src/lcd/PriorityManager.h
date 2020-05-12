#pragma once

#include "lcd/LcdInterface.h"

namespace lcd
{

enum class Priority : uint8_t
{
    LOW = 0,
    HIGH
};

class PriorityManager : public LcdInterface
{
public:
    PriorityManager( LcdInterface* lcd, Priority priority );
    ~PriorityManager() override;

    void setBackgroundColor( const Pixel& color ) override;
    void clear() override;
    void clearArea(  const Coordinates& corner1, const Coordinates& corner2 ) override;
    void fillArea( const Coordinates& corner1, const Coordinates& corner2, const Pixel& color ) override;

    void displayImage( uint8_t x, uint8_t y, const ImageLegacy& image) override;
    void displayImage( const Coordinates& coords, const Image& image, const Pixel& color ) override;
    void displayProgressArc( uint8_t x, uint8_t y, uint8_t position ) override;
    void initialize() override;
    void print( const etl::string_view& string, uint8_t x, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t y, Justification justification ) override;
    void print( const etl::string_view& string, uint8_t x, uint8_t y) override;
    void print( const etl::string_view& string, const Coordinates& coords, const Format& format ) override;
    void print( const etl::string_view& string, uint8_t y, const Format& format ) override;
    void setBacklightIntensity( uint8_t intensity ) override;

    uint8_t maximumBacklightIntensity() const override;
    uint16_t line( uint8_t lineNumber ) const override;

    void release() override;

    DrawInterface& draw() override;

private:
    bool checkPriority();

    LcdInterface& lcd_;

    const Priority priority_;
    static Priority currentPriorityFilter_;
};

}  // namespace lcd
