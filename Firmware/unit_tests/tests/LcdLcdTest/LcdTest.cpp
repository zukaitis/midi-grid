#include <gtest/gtest.h>

#include "io/lcd/Lcd.h"
#include "io/lcd/backlight/MockBacklight.h"
#include "io/lcd/MockDriver.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( BacklightConstructor, Create )
{
    lcd::MockBacklight mockBacklight;
    lcd::MockDriver mockDriver;

    const lcd::Lcd lcd( mockDriver, mockBacklight );
    SUCCEED();
}
