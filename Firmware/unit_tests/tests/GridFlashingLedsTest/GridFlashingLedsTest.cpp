#include <gtest/gtest.h>

#include "grid/FlashingLeds.h"

#include "grid/MockLedOutput.h"
#include "freertos/MockThread.h"

static const uint32_t delayPeriod = 250; // [ms]

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
    const grid::FlashingLeds flashingLeds( mockLedOutput );
    SUCCEED();
}

TEST( Run, CheckDimming )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::FlashingLeds flashingLeds( mockLedOutput );

    Color color1 = {2, 3, 33};
    Color color2 = {3, 2, 22};
    const grid::FlashingColors colors = {color1, color2};

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        flashingLeds.add( {i, 2}, colors );
    }

    for (uint8_t i = 0; i < 3; i++)
    {
        EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( testing::_, color1 ) ).Times( 8 );
        freertos::ThreadCaller::getInstance().Run();

        EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( testing::_, color2 ) ).Times( 8 );
        freertos::ThreadCaller::getInstance().Run();
    }
}

TEST( Run_add, CheckVectorLooping )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::FlashingLeds flashingLeds( mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        const grid::FlashingColors colors = {Color(4, 2, 0), Color(0, 2, 4)};
        flashingLeds.add( {2, 2}, colors );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 22; i++)
    {
        const grid::FlashingColors colors = {Color(8, 8, 8), Color(0, 8, 0)};
        flashingLeds.add( {3, 3}, colors );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 30 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 50; i++)
    {
        const grid::FlashingColors colors = {Color(7, 8, 9), Color(9, 8, 7)};
        flashingLeds.add( {4, 4}, colors );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 80 );
    freertos::ThreadCaller::getInstance().Run();

    const grid::FlashingColors colors = {Color(6, 6, 6), Color(9, 9, 9)};
    EXPECT_THROW( flashingLeds.add( {4, 4}, colors ), etl::vector_full );
}

TEST( remove, removeValues )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::FlashingLeds flashingLeds( mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        const grid::FlashingColors colors = {Color(4, 2, 0), Color(0, 2, 4)};
        flashingLeds.add( {i, 0}, colors );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    flashingLeds.remove( {3, 3} );

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    // expecting, that no elements were removed
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    for (uint8_t i = 0; i < 7; i++)
    {
        flashingLeds.remove( {i, 0} );

        EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
        EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 7 - i );
        freertos::ThreadCaller::getInstance().Run();
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    flashingLeds.remove( {7, 0} );

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 0 );
    freertos::ThreadCaller::getInstance().Run();
}

TEST( removeAll, removeEverything )
{
    grid::MockLedOutput mockLedOutput;

    EXPECT_CALL( freertos::MockThread::getInstance(), Start ).Times( 1 );
    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    grid::FlashingLeds flashingLeds( mockLedOutput );

    EXPECT_CALL( freertos::MockThread::getInstance(), Resume ).Times( 1 );
    for (uint8_t i = 0; i < 8; i++)
    {
        const grid::FlashingColors colors = {Color(4, 2, 0), Color(0, 2, 4)};
        flashingLeds.add( {i, 0}, colors );
    }

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 8 );
    freertos::ThreadCaller::getInstance().Run();

    EXPECT_CALL( freertos::MockThread::getInstance(), Suspend ).Times( 1 );
    flashingLeds.removeAll();

    EXPECT_CALL( freertos::MockThread::getInstance(), DelayUntil( delayPeriod ) ).Times( 1 );
    EXPECT_CALL( mockLedOutput, set( testing::_, testing::_ ) ).Times( 0 );
    freertos::ThreadCaller::getInstance().Run();
}
