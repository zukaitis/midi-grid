#include <gtest/gtest.h>
#include "io/grid/PulsingLeds.h"

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
