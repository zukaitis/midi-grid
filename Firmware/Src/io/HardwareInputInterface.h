#pragma once

template<class EventType>
class HardwareInputInterface
{
    virtual bool waitForInput( EventType& event ) = 0;

    virtual void discardPendingInput() = 0;
};
