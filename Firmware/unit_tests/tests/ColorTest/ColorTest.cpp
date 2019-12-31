#include <gtest/gtest.h>
#include "types/Color.h"

const uint8_t colorMaxValue = 64;

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( ColorConstructor, CreateWithParameters )
{
    const Color color( 4, 2, 0 );
    SUCCEED();
}

TEST( ColorConstructor, CreateWithoutParameters )
{
    Color color;
    SUCCEED();
}

TEST( ColorConstructor, TruncateValues )
{
    const Color color( 0xFF, 0xFF, 0xFF );
    
    EXPECT_EQ( colorMaxValue, color.red() );
    EXPECT_EQ( colorMaxValue, color.green() );
    EXPECT_EQ( colorMaxValue, color.blue() );
}

TEST( EqualityOperator, SameValues )
{
    const Color color1( 4, 2, 0 );
    const Color color2( 4, 2, 0 );
    
    EXPECT_TRUE( color1 == color2 );
}

TEST( EqualityOperator, DifferentPositions )
{
    const Color color1( 4, 2, 0 );
    const Color color2( 0, 4, 2 );
    
    EXPECT_FALSE( color1 == color2 );
}

TEST( AdditionOperator, Addition )
{
    const Color color1( 4, 2, 0 );
    const Color color2( 18, 2, 16 );

    const Color sum = color1 + color2;

    EXPECT_EQ( sum.red(), color1.red() + color2.red() );
    EXPECT_EQ( sum.green(), color1.green() + color2.green() );
    EXPECT_EQ( sum.blue(), color1.blue() + color2.blue() );
}

TEST( AdditionOperator, AdditionTruncated )
{
    const Color color1( 33, 45, 19 );
    const Color color2( 55, 20, 50 );

    const Color sum = color1 + color2;

    EXPECT_EQ( colorMaxValue, sum.red() );
    EXPECT_EQ( colorMaxValue, sum.green() );
    EXPECT_EQ( colorMaxValue, sum.blue() );
}

TEST( MultiplicationOperator, Multiplication )
{
    const Color color( 4, 2, 0 );
    const uint8_t multiplier = 7;

    const Color product = color * multiplier;

    EXPECT_EQ( product.red(), color.red() * multiplier );
    EXPECT_EQ( product.green(), color.green() * multiplier );
    EXPECT_EQ( product.blue(), color.blue() * multiplier );
}

TEST( MultiplicationOperator, MultiplicationTruncated )
{
    const Color color( 10, 11, 12 );
    const uint8_t multiplier = 7;

    const Color product = color * multiplier;

    EXPECT_EQ( colorMaxValue, product.red() );
    EXPECT_EQ( colorMaxValue, product.green() );
    EXPECT_EQ( colorMaxValue, product.blue() );
}

TEST( MultiplicationOperator, MultiplicationWithFloat )
{
    const Color color( 60, 25, 16 );
    const float multiplier = 0.5;

    const Color product = color * multiplier;

    EXPECT_EQ( product.red(), color.red() / 2 );
    EXPECT_EQ( product.green(), color.green() / 2 );
    EXPECT_EQ( product.blue(), color.blue() / 2 );
}

TEST( DivisionOperator, Division )
{
    const Color color( 54, 62, 16 );
    const uint8_t divisor = 8;

    const Color quotient = color / divisor;

    EXPECT_EQ( quotient.red(), color.red() / divisor );
    EXPECT_EQ( quotient.green(), color.green() / divisor );
    EXPECT_EQ( quotient.blue(), color.blue() / divisor );
}
