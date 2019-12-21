#pragma once

namespace grid
{

struct ButtonEvent;

class ButtonInputInterface
{
public:
    virtual ~ButtonInputInterface() {};

    virtual bool waitForEvent( ButtonEvent& event ) = 0;
    virtual void discardPendingEvents() = 0;
};

}