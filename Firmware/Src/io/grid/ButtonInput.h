#pragma once

#include "io/grid/GridInterface.h"
#include "hardware/grid/InputInterface.h"

#include "thread.hpp"
#include "queue.hpp"

#include "types/Coordinates.h"

namespace mcu
{
    class GlobalInterrupts;
}

namespace grid
{

typedef hardware::grid::InputBuffer InputBuffer;

class ButtonInput : public freertos::Thread
{
public:
    ButtonInput( hardware::grid::InputInterface& gridDriver, mcu::GlobalInterrupts& globalInterrupts );

    bool waitForEvent( ButtonEvent& event );
    void discardPendingEvents();

    void Run() override;
private:

    Coordinates calculatePhysicalCoordinates( const Coordinates& hardwareCoordinates ) const;

    void copyInputBuffers();
    void fillDebouncingBuffer( InputBuffer& buffer ) const;
    void fillChangesBuffer( InputBuffer& buffer ) const;

    etl::array<InputBuffer, 2> inputBuffers_;
    InputBuffer registeredInputBuffer_;

    mcu::GlobalInterrupts& globalInterrupts_;
    hardware::grid::InputInterface& gridDriver_;

    freertos::Queue events_;
};

}
