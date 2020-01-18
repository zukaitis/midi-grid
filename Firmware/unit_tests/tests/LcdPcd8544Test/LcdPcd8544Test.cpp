#include <gtest/gtest.h>

#include "lcd/84x48_mono/Pcd8544.h"
#include "hardware/lcd/MockSpi.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( BacklightConstructor, Create )
{
    hardware::lcd::MockSpi mockSpi;

    const lcd::Pcd8544 pcd8544( mockSpi );
    SUCCEED();
}
