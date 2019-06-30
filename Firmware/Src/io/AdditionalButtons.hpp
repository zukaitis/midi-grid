#ifndef GRID_SWITCHES_HPP_
#define GRID_SWITCHES_HPP_

#include "Types.h"
#include "thread.hpp"
#include "queue.hpp"

namespace grid
{
    class GridDriver;
}

// class used to acquire values from two additional buttons and rotary encoders
class AdditionalButtons : private freertos::Thread
{
public:
    AdditionalButtons( grid::GridDriver& gridControl );
    ~AdditionalButtons();

    enum Button : uint8_t
    {
        extraNoteButton = 0,
        internalMenuButton = 1U,
        numberOfButtons
    };

    struct Event
    {
        ButtonAction action;
        Button button;
    };

    void discardAllPendingEvents();
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

    grid::GridDriver& gridDriver_;
    freertos::Queue inputEvents_;

    bool registeredButtonInput_[numberOfButtons];
};

#endif // GRID_SWITCHES_HPP_
