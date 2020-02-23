#pragma once

#include "grid/GridInterface.h"
#include <etl/array.h>

namespace grid
{

class ButtonInputInterface;
class LedOutputInterface;
class FlashingLedsInterface;
class PulsingLedsInterface;

class Grid : public GridInterface
{
public:
    Grid( ButtonInputInterface* buttonInput,
        LedOutputInterface* ledOutput,
        FlashingLedsInterface* flashingLeds,
        PulsingLedsInterface* pulsingLeds );

    bool waitForInput( ButtonEvent* event ) override;
    void discardPendingInput() override;

    Color getLedColor( const Coordinates& coordinates ) const override;

    void setLed( const Coordinates& coordinates, const Color& color, LedLightingType lightingType ) override;

    void turnAllLedsOff() override;

private:
    ButtonInputInterface& buttonInput_;
    LedOutputInterface& ledOutput_;
    FlashingLedsInterface& flashingLeds_;
    PulsingLedsInterface& pulsingLeds_;

    struct Led
    {
        Color color;
        LedLightingType lightingType;
    };
    
    etl::array<etl::array<Led, numberOfRows>, numberOfColumns> led_;
};

}  // namespace grid
