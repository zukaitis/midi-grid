#include <gtest/gtest.h>

#include "lcd/backlight/Backlight.h"
#include "hardware/lcd/MockBacklightDriver.h"
#include "freertos/MockThread.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( BacklightConstructor, Create )
{
    hardware::lcd::MockBacklightDriver mockBacklightDriver;

    const lcd::Backlight backlight( mockBacklightDriver );
    SUCCEED();
}
