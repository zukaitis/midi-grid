#pragma once

#include "hardware/grid/TestingInterface.h"
#include "testing/TestingInterface.h"

namespace hardware
{
    namespace grid
    {
        class TestingInterface;
    }
}

namespace testing
{

class Testing : public TestingInterface
{
public:
    explicit Testing( hardware::grid::TestingInterface* gridDriver );
    void injectButtonPress( const Coordinates& coords ) override;

private:
    static Coordinates calculateHardwareCoordinates( const Coordinates& coordinates );

    hardware::grid::TestingInterface& gridDriver_;
};

}  // namespace testing
