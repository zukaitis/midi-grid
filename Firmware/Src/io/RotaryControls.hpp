#ifndef ROTARY_CONTROLS_HPP_
#define ROTARY_CONTROLS_HPP_

#include "thread.hpp"
#include "queue.hpp"

namespace hardware
{
    namespace grid
    {
        class GridDriver;
    }
}

class RotaryControls : private freertos::Thread
{
public:
    RotaryControls( hardware::grid::GridDriver& gridControl );
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
    };

    inline void discardPendingInput()
    {
        discardAllPendingEvents();
    };

private:
    void Run();

    int8_t calculateVelocityMultiplier( const uint32_t intervalMs ) const;

    inline void notifyFromISRWrapper()
    {
        NotifyFromISR();
    }

    hardware::grid::GridDriver& gridDriver_;
    freertos::Queue inputEvents_;
};

#endif // ROTARY_CONTROLS_HPP_
