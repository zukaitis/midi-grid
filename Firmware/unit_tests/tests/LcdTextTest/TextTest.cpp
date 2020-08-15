#include <gtest/gtest.h>

#include "lcd/text/Format.h"
#include "lcd/text/Text.h"
#include "lcd/image/ImageInterface.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( Constructor, Create )
{
    lcd::ImageInterface mockImageInterface;
    lcd::Text text( &mockImageInterface );

    SUCCEED();
}

TEST( Print, EmptyString )
{
    lcd::ImageInterface mockImageInterface;
    lcd::Text text( &mockImageInterface );

    text.print( "", 0, lcd::Format() );

    SUCCEED();
}
