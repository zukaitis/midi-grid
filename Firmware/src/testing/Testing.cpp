#include "testing/Testing.h"
#include "hardware/grid/TestingInterface.h"
#include "grid/GridInterface.h"

namespace testing
{

Testing::Testing( hardware::grid::TestingInterface* gridDriver ):
    gridDriver_( *gridDriver )
{
}

void Testing::injectButtonPress( const Coordinates& coords )
{
    if (coords < grid::gridLimits)
    {
        const Coordinates hardwareCoords = calculateHardwareCoordinates( coords );
        gridDriver_.injectButtonPress( hardwareCoords );
    }
}

Coordinates Testing::calculateHardwareCoordinates( const Coordinates& coordinates )
{
    Coordinates hardwareCoordinates = coordinates;

    if (coordinates.y >= hardware::grid::numberOfRows)
    {
        hardwareCoordinates.x += grid::numberOfColumns;
        hardwareCoordinates.y %= hardware::grid::numberOfRows;
    }
    hardwareCoordinates.x = (hardwareCoordinates.x + hardware::grid::numberOfColumns) % hardware::grid::numberOfColumns;

    return hardwareCoordinates;
}

}  // namespace testing
