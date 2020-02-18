#pragma once

#include "grid/ButtonInputInterface.h"
#include "grid/GridInterface.h"
#include "hardware/grid/InputInterface.h"

#include <freertos/thread.hpp>
#include <freertos/queue.hpp>
#include <freertos/semaphore.hpp>

#include "types/Coordinates.h"

namespace mcu
{
    class GlobalInterruptsInterface;
}

namespace grid
{

typedef hardware::grid::InputBuffer InputBuffer;

class ButtonInput : public ButtonInputInterface, private freertos::Thread
{
public:
    ButtonInput( hardware::grid::InputInterface* gridDriver, mcu::GlobalInterruptsInterface* globalInterrupts );

    bool waitForEvent( ButtonEvent* event ) override;
    void discardPendingEvents() override;

private:
    void Run() override;

    Coordinates calculatePhysicalCoordinates( const Coordinates& hardwareCoordinates ) const;

    void copyInputBuffers();
    void fillDebouncingBuffer( InputBuffer& buffer ) const;
    void fillChangesBuffer( InputBuffer& buffer ) const;

    etl::array<InputBuffer, 2> inputBuffers_;
    InputBuffer registeredInputBuffer_;

    mcu::GlobalInterruptsInterface& globalInterrupts_;
    hardware::grid::InputInterface& gridDriver_;

    freertos::Queue events_;
    freertos::BinarySemaphore changesAvailable_;
};

}  // namespace grid
