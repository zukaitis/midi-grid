#include <gtest/gtest.h>

#include "io/grid/ButtonInput.h"
#include "hardware/grid/MockGridDriver.h"
#include "system/MockGlobalInterrupts.h"
#include "freertos/MockThread.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( LedOutputConstructor, Create )
{
    hardware::grid::MockGridDriver mockGridDriver;
    mcu::MockGlobalInterrupts mockGlobalInterrupts;

    EXPECT_CALL( mockGridDriver, addThreadToNotify( testing::_ ) );
    EXPECT_CALL( freertos::MockThread::getInstance(), Start );
    const grid::ButtonInput buttonInput( mockGridDriver, mockGlobalInterrupts);
}
