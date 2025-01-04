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

ShapesInterface PriorityManager::emptyShapesInterface_ = {};
TextInterface PriorityManager::emptyTextInterface_ = {};
ImageInterface PriorityManager::emptyImageInterface_ = {};
BacklightInterface PriorityManager::emptyBacklightInterface_ = {};

void PriorityManager::initialize()
{
    lcd_.initialize();
}

BacklightInterface& PriorityManager::backlight()
{
    if (checkPriority())
    {
        return lcd_.backlight();
    }
    return emptyBacklightInterface_;
}

ImageInterface& PriorityManager::image()
{
    if (checkPriority())
    {
        return lcd_.image();
    }
    return emptyImageInterface_;
}

ShapesInterface& PriorityManager::shapes()
{
    if (checkPriority())
    {
        return lcd_.shapes();
    }
    return emptyShapesInterface_;
}

TextInterface& PriorityManager::text()
{
    if (checkPriority())
    {
        return lcd_.text();
    }
    return emptyTextInterface_;
}

uint16_t PriorityManager::width()
{
    return lcd_.width();
}

uint16_t PriorityManager::height()
{
    return lcd_.height();
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
