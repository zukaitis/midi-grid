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

    void initialize() override;

    BacklightInterface& backlight() override;
    ImageInterface& image() override;
    ShapesInterface& shapes() override;
    TextInterface& text() override;

    uint16_t width() override;
    uint16_t height() override;

    void release() override;

private:
    bool checkPriority();

    LcdInterface& lcd_;

    const Priority priority_;
    static Priority currentPriorityFilter_;

    static ShapesInterface emptyShapesInterface_;
    static TextInterface emptyTextInterface_;
    static ImageInterface emptyImageInterface_;
    static BacklightInterface emptyBacklightInterface_;
};

}  // namespace lcd
