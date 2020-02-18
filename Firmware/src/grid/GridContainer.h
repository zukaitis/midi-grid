#pragma once

#include "grid/ButtonInput.h"
#include "grid/LedOutput.h"
#include "grid/FlashingLeds.h"
#include "grid/PulsingLeds.h"
#include "grid/Grid.h"

namespace grid
{

class GridContainer
{
public:
    GridContainer( hardware::grid::InputInterface* gridDriverInput, hardware::grid::OutputInterface* gridDriverOutput,
        mcu::GlobalInterruptsInterface* globalInterrupts );
    virtual ~GridContainer() = default;

    GridInterface& getGrid();

private:
    ButtonInput buttonInput_;
    LedOutput ledOutput_;
    FlashingLeds flashingLeds_;
    PulsingLeds pulsingLeds_;
    Grid grid_;
};

}  // namespace grid
