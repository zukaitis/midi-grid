#include "grid/LedOutput.h"

#include "grid/GridInterface.h"
#include "hardware/grid/OutputInterface.h"
#include "types/Color.h"
#include "types/Coordinates.h"
#include <etl/array.h>

namespace grid
{

static const uint8_t kNumberOfDiffusedColumns = 8;
static const uint8_t hardwareColumnOffset = 1;

static const etl::array<uint16_t, 64> kBrightnessDiffused = {
    0, 338, 635, 943, 1273, 1627, 2001, 2393,
    2805, 3237, 3683, 4149, 4627, 5121, 5641, 6157,
    6713, 7259, 7823, 8400, 9007, 9603, 10229, 10856,
    11501, 12149, 12815, 13499, 14204, 14892, 15571, 16279,
    17027, 17769, 18506, 19267, 20048, 20844, 21617, 22436,
    23219, 24066, 24897, 25725, 26599, 27430, 28301, 29198,
    30947, 31835, 32790, 33683, 34601, 35567, 36579, 37515,
    38478, 39415, 40403, 41358, 42373, 43359, 44409, 46000 };

static const etl::array<uint16_t, 64> kBrightnessDirect = {
    0, 223, 308, 397, 494, 598, 709, 825,
    947, 1075, 1208, 1348, 1491, 1639, 1793, 1950,
    2113, 2279, 2448, 2621, 2802, 2981, 3164, 3354,
    3542, 3738, 3931, 4133, 4337, 4542, 4748, 4952,
    5166, 5382, 5591, 5809, 6032, 6259, 6473, 6699,
    6922, 7155, 7385, 7607, 7856, 8079, 8319, 8551,
    8783, 9021, 9265, 9500, 9753, 9995, 10233, 10489,
    10737, 10989, 11213, 11729, 11987, 12203, 12480, 12741 };

LedOutput::LedOutput( hardware::grid::OutputInterface* gridDriver ):
    gridDriver_( *gridDriver )
{
}

void LedOutput::set( const Coordinates& coordinates, const Color& color )
{
    const Coordinates hardwareCoordinates = calculateHardwareCoordinates( coordinates );

    if (coordinates.x < kNumberOfDiffusedColumns)
    {
        gridDriver_.setRedOutput( hardwareCoordinates, kBrightnessDiffused.at(color.red()>>2U) );
        gridDriver_.setGreenOutput( hardwareCoordinates, kBrightnessDiffused.at(color.green()>>2U) );
        gridDriver_.setBlueOutput( hardwareCoordinates, kBrightnessDiffused.at(color.blue()>>2U) );
    }
    else
    {
        gridDriver_.setRedOutput( hardwareCoordinates, kBrightnessDirect.at(color.red()>>2U) );
        gridDriver_.setGreenOutput( hardwareCoordinates, kBrightnessDirect.at(color.green()>>2U) );
        gridDriver_.setBlueOutput( hardwareCoordinates, kBrightnessDirect.at(color.blue()>>2U) );
    }
}

void LedOutput::setAllOff()
{
    gridDriver_.setAllOff();
}

Coordinates LedOutput::calculateHardwareCoordinates( const Coordinates& coordinates ) const
{
    Coordinates hardwareCoordinates = coordinates;

    if (coordinates.y >= hardware::grid::numberOfRows)
    {
        hardwareCoordinates.x += numberOfColumns;
        hardwareCoordinates.y %= hardware::grid::numberOfRows;
    }
    hardwareCoordinates.x = (hardwareCoordinates.x + hardware::grid::numberOfColumns - hardwareColumnOffset) % hardware::grid::numberOfColumns;

    return hardwareCoordinates;
}

}  // namespace grid
