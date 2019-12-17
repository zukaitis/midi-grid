#include <gtest/gtest.h>

#define private public
#include "io/grid/PulsingLeds.h"
#undef private

#include "io/grid/MockLedOutput.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( PulsingLedsConstructor, Create )
{
    grid::MockLedOutput mockLedOutput;
    const grid::PulsingLeds pulsingLeds( mockLedOutput );
    SUCCEED();
}

TEST( setOutputValues, CheckDimming )
{
    grid::MockLedOutput mockLedOutput;
    grid::PulsingLeds pulsingLeds( mockLedOutput );

    const uint8_t valueRed = 64;
    const uint8_t valueGreen = 27;
    const uint8_t valueBlue = 52;
    const Coordinates coords = {0, 0};

    EXPECT_CALL( pulsingLeds, Resume ).Times(1);
    pulsingLeds.add( coords, {valueRed, valueGreen, valueBlue} );

    for (uint8_t step = 0; step < 4; step++)
    {
        const Color expectedColor = Color(
            ((step + 1) * valueRed) / 4,
            ((step + 1) * valueGreen) / 4,
            ((step + 1) * valueBlue) / 4 );

        EXPECT_CALL( mockLedOutput, set( coords, expectedColor ) ).Times(1);
        pulsingLeds.setOutputValues();
    }

    for (uint8_t step = 4; step < 15; step++)
    {
        const Color expectedColor = Color(
            ((19 - step) * valueRed) / 16,
            ((19 - step) * valueGreen) / 16,
            ((19 - step) * valueBlue) / 16 );

        EXPECT_CALL( mockLedOutput, set( coords, expectedColor ) ).Times(1);
        pulsingLeds.setOutputValues();
    }
}

TEST( setOutputValues_add, CheckVectorLooping )
{
    grid::MockLedOutput mockLedOutput;
    grid::PulsingLeds pulsingLeds( mockLedOutput );


    EXPECT_CALL( pulsingLeds, Resume ).Times(1);
    for (uint8_t i = 0; i < 8; i++)
    {
        pulsingLeds.add( {2, 2}, {4, 2, 0} );
    }

    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    pulsingLeds.setOutputValues();

    for (uint8_t i = 0; i < 22; i++)
    {
        pulsingLeds.add( {3, 3}, {8, 8, 8} );
    }

    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 30 );
    pulsingLeds.setOutputValues();

    for (uint8_t i = 0; i < 50; i++)
    {
        pulsingLeds.add( {4, 4}, {7, 8, 9} );
    }

    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 80 );
    pulsingLeds.setOutputValues();

    EXPECT_THROW( pulsingLeds.add( {4, 4}, {7, 8, 9} ), etl::vector_full );
}
