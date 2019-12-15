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
    
    EXPECT_EQ( colorMaxValue, color.getRed() );
    EXPECT_EQ( colorMaxValue, color.getGreen() );
    EXPECT_EQ( colorMaxValue, color.getBlue() );
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

    EXPECT_EQ( sum.getRed(), color1.getRed() + color2.getRed() );
    EXPECT_EQ( sum.getGreen(), color1.getGreen() + color2.getGreen() );
    EXPECT_EQ( sum.getBlue(), color1.getBlue() + color2.getBlue() );
}

TEST( AdditionOperator, AdditionTruncated )
{
    const Color color1( 33, 45, 19 );
    const Color color2( 55, 20, 50 );

    const Color sum = color1 + color2;

    EXPECT_EQ( colorMaxValue, sum.getRed() );
    EXPECT_EQ( colorMaxValue, sum.getGreen() );
    EXPECT_EQ( colorMaxValue, sum.getBlue() );
}

TEST( MultiplicationOperator, Multiplication )
{
    const Color color( 4, 2, 0 );
    const uint8_t multiplier = 7;

    const Color product = color * multiplier;

    EXPECT_EQ( product.getRed(), color.getRed() * multiplier );
    EXPECT_EQ( product.getGreen(), color.getGreen() * multiplier );
    EXPECT_EQ( product.getBlue(), color.getBlue() * multiplier );
}

TEST( MultiplicationOperator, MultiplicationTruncated )
{
    const Color color( 10, 11, 12 );
    const uint8_t multiplier = 7;

    const Color product = color * multiplier;

    EXPECT_EQ( colorMaxValue, product.getRed() );
    EXPECT_EQ( colorMaxValue, product.getGreen() );
    EXPECT_EQ( colorMaxValue, product.getBlue() );
}

TEST( MultiplicationOperator, MultiplicationWithFloat )
{
    const Color color( 60, 25, 16 );
    const float multiplier = 0.5;

    const Color product = color * multiplier;

    EXPECT_EQ( product.getRed(), color.getRed() / 2 );
    EXPECT_EQ( product.getGreen(), color.getGreen() / 2 );
    EXPECT_EQ( product.getBlue(), color.getBlue() / 2 );
}

TEST( DivisionOperator, Division )
{
    const Color color( 54, 62, 16 );
    const uint8_t divisor = 8;

    const Color quotient = color / divisor;

    EXPECT_EQ( quotient.getRed(), color.getRed() / divisor );
    EXPECT_EQ( quotient.getGreen(), color.getGreen() / divisor );
    EXPECT_EQ( quotient.getBlue(), color.getBlue() / divisor );
}
