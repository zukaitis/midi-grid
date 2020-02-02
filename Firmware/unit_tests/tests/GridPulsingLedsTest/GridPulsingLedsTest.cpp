#include <gtest/gtest.h>

#include "grid/PulsingLeds.h"

#include "grid/MockLedOutput.h"
#include "freertos/MockThread.h"

const uint32_t delayPeriod = 67; // [ms]

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( PulsingLedsConstructor, Create )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    const grid::PulsingLeds pulsingLeds( &mockLedOutput );
    SUCCEED();
}

TEST( Run, CheckDimming )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::PulsingLeds pulsingLeds( &mockLedOutput );

    const uint8_t valueRed = 64;
    const uint8_t valueGreen = 27;
    const uint8_t valueBlue = 52;
    const Coordinates coords = {0, 0};

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    pulsingLeds.add( coords, {valueRed, valueGreen, valueBlue} );

    for (uint8_t step = 0; step < 4; step++)
    {
        const Color expectedColor = Color(
            ((step + 1) * valueRed) / 4,
            ((step + 1) * valueGreen) / 4,
            ((step + 1) * valueBlue) / 4 );

        EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( coords, expectedColor ) ).Times( 1 );
        freertos::ThreadCaller::getInstance().Run();
    }

    for (uint8_t step = 4; step < 15; step++)
    {
        const Color expectedColor = Color(
            ((19 - step) * valueRed) / 16,
            ((19 - step) * valueGreen) / 16,
            ((19 - step) * valueBlue) / 16 );

        EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( coords, expectedColor ) ).Times( 1 );
        freertos::ThreadCaller::getInstance().Run();
    }
}

TEST( Run_add, CheckVectorLooping )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::PulsingLeds pulsingLeds( &mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        pulsingLeds.add( {2, 2}, {4, 2, 0} );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 22; i++)
    {
        pulsingLeds.add( {3, 3}, {8, 8, 8} );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 30 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 50; i++)
    {
        pulsingLeds.add( {4, 4}, {7, 8, 9} );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 80 );
    freertos::ThreadCaller::getInstance().Run();

    EXPECT_THROW( pulsingLeds.add( {4, 4}, {7, 8, 9} ), etl::vector_full );
}

TEST( remove, removeValues )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::PulsingLeds pulsingLeds( &mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        pulsingLeds.add( {i, 0}, {4, 2, 0} );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    pulsingLeds.remove( {3, 3} );

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    // expecting, that no elements were removed
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 7; i++)
    {
        pulsingLeds.remove( {i, 0} );

       EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 7 - i );
        freertos::ThreadCaller::getInstance().Run();
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    pulsingLeds.remove( {7, 0} );

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 0 );
    freertos::ThreadCaller::getInstance().Run();
}

TEST( removeAll, removeEverything )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::PulsingLeds pulsingLeds( &mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        pulsingLeds.add( {i, 0}, {4, 2, 0} );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    pulsingLeds.removeAll();

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 0 );
    freertos::ThreadCaller::getInstance().Run();
}
