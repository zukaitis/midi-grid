#pragma once

#include "io/grid/LedOutputInterface.h"

namespace hardware
{
    namespace grid
    {
        class OutputInterface;
    }
}

namespace grid
{

class LedOutput : public LedOutputInterface
{
public:
    LedOutput( hardware::grid::OutputInterface& gridDriver );

    void set( const Coordinates& coordinates, const Color& color ) override;
    void setAllOff() override;

private:
    Coordinates calculateHardwareCoordinates( const Coordinates& coordinates ) const;

    hardware::grid::OutputInterface& gridDriver_;
};

}