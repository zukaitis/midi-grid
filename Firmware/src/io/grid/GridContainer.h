#pragma once

#include "io/grid/ButtonInput.h"
#include "io/grid/LedOutput.h"
#include "io/grid/FlashingLeds.h"
#include "io/grid/PulsingLeds.h"
#include "io/grid/Grid.h"

namespace grid
{

class GridContainer
{
public:
    GridContainer( hardware::grid::InputInterface& gridDriverInput, hardware::grid::OutputInterface& gridDriverOutput,
        mcu::GlobalInterruptsInterface& globalInterrupts );
    virtual ~GridContainer() = default;

    GridInterface& getGrid();

private:
    ButtonInput buttonInput_;
    LedOutput ledOutput_;
    FlashingLeds flashingLeds_;
    PulsingLeds pulsingLeds_;
    Grid grid_;
};

}