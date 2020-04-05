#include "lcd/PriorityManager.h"

#include <functional>

namespace lcd
{

PriorityManager::PriorityManager( LcdInterface* lcd, Priority priority ):
    lcd_( *lcd ),
    priority_( priority )
{
}

PriorityManager::~PriorityManager() = default;

Priority PriorityManager::currentPriorityFilter_ = Priority::LOW;

void PriorityManager::clear()
{
    if (checkPriority())
    {
        lcd_.clear();
    }
}

void PriorityManager::clearArea( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 )
{
    if (checkPriority())
    {
        lcd_.clearArea( x1, y1, x2, y2 );
    }
}

void PriorityManager::displayImage( uint8_t x, uint8_t y, const Image& image )
{
    if (checkPriority())
    {
        lcd_.displayImage( x, y, image );
    }
}

void PriorityManager::displayProgressArc( uint8_t x, uint8_t y, uint8_t position )
{
    if (checkPriority())
    {
        lcd_.displayProgressArc( x, y, position );
    }
}

void PriorityManager::initialize()
{
    if (checkPriority())
    {
        lcd_.initialize();
    }
}

void PriorityManager::print( const char* string, uint8_t x, uint8_t y, Justification justification )
{
    if (checkPriority())
    {
        lcd_.print( string, x, y, justification );
    }
}

void PriorityManager::print( const char* string, uint8_t x, uint8_t y )
{
    if (checkPriority())
    {
        lcd_.print( string, x, y );
    }
}

void PriorityManager::printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y, Justification justification )
{
    if (checkPriority())
    {
        lcd_.printNumberInBigDigits( number, x, y, justification );
    }
}

void PriorityManager::printNumberInBigDigits( uint16_t number, uint8_t x, uint8_t y )
{
    if (checkPriority())
    {
        lcd_.printNumberInBigDigits( number, x, y );
    }
}

void PriorityManager::setBacklightIntensity( uint8_t intensity )
{
    if (checkPriority())
    {
        lcd_.setBacklightIntensity( intensity );
    }
}

uint16_t PriorityManager::right() const
{
    return lcd_.right();
}

uint8_t PriorityManager::maximumBacklightIntensity() const
{
    return lcd_.maximumBacklightIntensity();
}

uint8_t PriorityManager::horizontalCenter() const
{
    return lcd_.horizontalCenter();
}

void PriorityManager::release()
{
    if (checkPriority())
    {
        currentPriorityFilter_ = Priority::LOW;
    }
}

bool PriorityManager::checkPriority()
{
    bool allow = false;
    if (priority_ >= currentPriorityFilter_)
    {
        currentPriorityFilter_ = priority_;
        allow = true;
    }
    return allow;
}

}  // namespace lcd
