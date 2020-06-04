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

void PriorityManager::setBackgroundColor( const Color& color )
{
    if (checkPriority())
    {
        lcd_.setBackgroundColor( color );
    }
}

void PriorityManager::clear()
{
    if (checkPriority())
    {
        lcd_.clear();
    }
}

void PriorityManager::clearArea( const Coordinates& corner1, const Coordinates& corner2 )
{
    if (checkPriority())
    {
        lcd_.clearArea( corner1, corner2 );
    }
}

void PriorityManager::fillArea( const Coordinates& corner1, const Coordinates& corner2, const Color& color )
{
    if (checkPriority())
    {
        lcd_.fillArea( corner1, corner2, color );
    }
}

void PriorityManager::displayImage( uint8_t x, uint8_t y, const ImageLegacy& image )
{
    if (checkPriority())
    {
        lcd_.displayImage( x, y, image );
    }
}

void PriorityManager::displayImage( const Coordinates& coords, const ImageMono& image, const Color& color )
{
    if (checkPriority())
    {
        lcd_.displayImage( coords, image, color );
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

void PriorityManager::print( const etl::string_view& string, uint8_t x, uint8_t y, Justification justification )
{
    if (checkPriority())
    {
        lcd_.print( string, x, y, justification );
    }
}

void PriorityManager::print( const etl::string_view& string, uint8_t y, Justification justification )
{
    if (checkPriority())
    {
        lcd_.print( string, y, justification );
    }
}

void PriorityManager::print( const etl::string_view& string, uint8_t x, uint8_t y )
{
    if (checkPriority())
    {
        lcd_.print( string, x, y );
    }
}

uint16_t PriorityManager::print( const etl::string_view& string, const Coordinates& coords, const Format& format )
{
    uint16_t result = 0;
    if (checkPriority())
    {
        result = lcd_.print( string, coords, format );
    }
    return result;
}

void PriorityManager::print( const etl::string_view& string, const uint8_t y, const Format& format )
{
    if (checkPriority())
    {
        lcd_.print( string, y, format );
    }
}

void PriorityManager::setBacklightIntensity( uint8_t intensity )
{
    if (checkPriority())
    {
        lcd_.setBacklightIntensity( intensity );
    }
}

uint8_t PriorityManager::maximumBacklightIntensity() const
{
    return lcd_.maximumBacklightIntensity();
}

uint16_t PriorityManager::line( uint8_t lineNumber ) const
{
    return lcd_.line( lineNumber );
}

void PriorityManager::release()
{
    if (checkPriority())
    {
        currentPriorityFilter_ = Priority::LOW;
    }
}

DrawInterface& PriorityManager::draw()
{
    return lcd_.draw();
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
