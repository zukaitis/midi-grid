#include <gtest/gtest.h>

#include "grid/LedOutput.h"
#include "hardware/grid/MockGridDriver.h"
#include "types/Color.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( LedOutputConstructor, Create )
{
    hardware::grid::MockGridDriver mockGridDriver;
    const grid::LedOutput ledOutput( &mockGridDriver );
    SUCCEED();
}

TEST( set, CallGridDriverSet )
{
    hardware::grid::MockGridDriver mockGridDriver;
    grid::LedOutput ledOutput( &mockGridDriver );

    EXPECT_CALL( mockGridDriver, setRedOutput( testing::_, testing::_ ) ).Times( 1 );
    EXPECT_CALL( mockGridDriver, setGreenOutput( testing::_, testing::_ ) ).Times( 1 );
    EXPECT_CALL( mockGridDriver, setBlueOutput( testing::_, testing::_ ) ).Times( 1 );
    ledOutput.set( {2, 3}, {4, 2, 0} );
}

TEST( set, CheckOutputValues )
{
    const uint32_t directMaximumValue = 12741;
    const uint32_t diffusedMaximumValue = 46000;
    const uint8_t numberOfDiffusedColumns = 8;

    hardware::grid::MockGridDriver mockGridDriver;
    grid::LedOutput ledOutput( &mockGridDriver );

    for (uint8_t x = 0; x < 10; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            const uint32_t expectedValue = (x < numberOfDiffusedColumns) ? diffusedMaximumValue : directMaximumValue;

            EXPECT_CALL( mockGridDriver, setRedOutput( testing::_, expectedValue ) ).Times( 1 );
            EXPECT_CALL( mockGridDriver, setGreenOutput( testing::_, expectedValue ) ).Times( 1 );
            EXPECT_CALL( mockGridDriver, setBlueOutput( testing::_, expectedValue ) ).Times( 1 );
            ledOutput.set( {x, y}, Color(255, 255, 255) );
        }
    }
}

TEST( set, CheckOutputCoordinates )
{
    hardware::grid::MockGridDriver mockGridDriver;
    grid::LedOutput ledOutput( &mockGridDriver );

    for (uint8_t x = 0; x < 10; x++)
    {
        for (uint8_t y = 0; y < 8; y++)
        {
            const Coordinates expectedCoordinates = {
                .x = static_cast<uint8_t>((x + (y / 4) * 10 + 20 - 1) % 20),
                .y = static_cast<uint8_t>(y % 4) };


            EXPECT_CALL( mockGridDriver, setRedOutput( expectedCoordinates, testing::_ ) ).Times( 1 );
            EXPECT_CALL( mockGridDriver, setGreenOutput( expectedCoordinates, testing::_ ) ).Times( 1 );
            EXPECT_CALL( mockGridDriver, setBlueOutput( expectedCoordinates, testing::_ ) ).Times( 1 );
            ledOutput.set( {x, y}, {9, 9, 9} );
        }
    }
}

TEST( setAllOff, CallGridDriverSetAllOff )
{
    hardware::grid::MockGridDriver mockGridDriver;
    grid::LedOutput ledOutput( &mockGridDriver );

    EXPECT_CALL( mockGridDriver, setAllOff() ).Times( 1 );
    ledOutput.setAllOff();
}
