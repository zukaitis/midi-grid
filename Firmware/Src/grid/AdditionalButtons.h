#ifndef GRID_SWITCHES_H_
#define GRID_SWITCHES_H_

#include "Types.h"
#include "thread.hpp"
#include "queue.hpp"

namespace grid
{

class GridDriver;

// class used to acquire values from two additional buttons and rotary encoders
class AdditionalButtons : private freertos::Thread
{
public:
    AdditionalButtons( GridDriver& gridControl );
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
    bool getEvent( Event& event );

private:
    void Run();

    inline void notifyFromISRWrapper()
    {
        NotifyFromISR();
    }

    GridDriver& gridDriver_;
    freertos::Queue inputEvents_;

    bool registeredButtonInput_[numberOfButtons];
};

} // namespace grid

#endif // GRID_SWITCHES_H_
