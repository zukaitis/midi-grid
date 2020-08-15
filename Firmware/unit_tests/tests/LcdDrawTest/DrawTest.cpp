#include <gtest/gtest.h>

#include "lcd/Lcd.h"
#include "lcd/backlight/MockBacklight.h"
#include "lcd/MockDriver.h"

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
