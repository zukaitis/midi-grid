#include <gtest/gtest.h>
#include "Color.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( ColorConstructor, CreateWithParameters )
{
    Color color( 4, 2, 0 );
    SUCCEED();
}

TEST( ColorConstructor, CreateWithoutParameters )
{
    Color color;
    SUCCEED();
}

TEST( ColorConstructor, TruncateValues )
{
    Color color( 0xFF, 0xFF, 0xFF );
    
    EXPECT_EQ( 64, color.getRed() );
    EXPECT_EQ( 64, color.getGreen() );
    EXPECT_EQ( 64, color.getBlue() );
}

TEST( EqualityOperator, SameValues )
{
    Color color1( 4, 2, 0 );
    Color color2( 4, 2, 0 );
    
    EXPECT_TRUE( color1 == color2 );
}

TEST( EqualityOperator, DifferentPositions )
{
    Color color1( 4, 2, 0 );
    Color color2( 0, 4, 2 );
    
    EXPECT_FALSE( color1 == color2 );
}

