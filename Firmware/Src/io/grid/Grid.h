#pragma once

#include "io/grid/GridInterface.h"
#include "io/grid/ButtonInput.h"
#include "io/grid/LedOutput.h"
#include "io/grid/FlashingLeds.h"
#include "io/grid/PulsingLeds.h"

namespace grid
{

class Grid : public GridInterface
{
public:
    Grid( hardware::grid::InputInterface& gridDriverInput, hardware::grid::OutputInterface& gridDriverOutput, mcu::GlobalInterrupts& globalInterrupts );

    bool waitForInput( ButtonEvent& event ) override;
    void discardPendingInput() override;

    Color getLedColor( const Coordinates& coordinates ) const override;

    void setLed( const Coordinates& coordinates, const Color& color, const LedLightingType lightingType = LedLightingType::LIGHT ) override;

    void turnAllLedsOff() override;

private:
    ButtonInput buttonInput_;
    LedOutput ledOutput_;
    FlashingLeds flashingLeds_;
    PulsingLeds pulsingLeds_;

    struct Led
    {
        Color color;
        LedLightingType lightingType;
    };
    
    etl::array<etl::array<Led, numberOfRows>, numberOfColumns> led_;
};

}
