#ifndef ROTARY_CONTROLS_H_
#define ROTARY_CONTROLS_H_

#include "thread.hpp"
#include "queue.hpp"

namespace grid
{

class GridDriver;

class RotaryControls : private freertos::Thread
{
public:
    RotaryControls( GridDriver& gridControl );
    ~RotaryControls();

    void discardAllPendingEvents();

    struct Event
    {
        int8_t steps;
        uint8_t control;
    };

    bool waitForEvent( Event& event );

    inline bool waitForInput( Event& event )
    {
        return waitForEvent( event );
    }

private:
    void Run();

    int8_t calculateVelocityMultiplier( const uint32_t intervalMs ) const;

    inline void notifyFromISRWrapper()
    {
        NotifyFromISR();
    }

    GridDriver& gridDriver_;
    freertos::Queue inputEvents_;
};

} // namespace

#endif // ROTARY_CONTROLS_H_
