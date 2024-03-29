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
    bool checkPriority();

    LcdInterface& lcd_;

    const Priority priority_;
    static Priority currentPriorityFilter_;
};

}
