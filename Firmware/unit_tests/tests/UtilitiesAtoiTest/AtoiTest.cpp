#include <gtest/gtest.h>
#include "etl/string_view.h"
#include "utilities/atoi.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( Atoi, Convert )
{
    EXPECT_EQ( 1234567890, utilities::atoi( etl::string_view( "1234567890" ) ) );
    EXPECT_EQ( 0, utilities::atoi( etl::string_view( "0" ) ) );
    EXPECT_EQ( 7, utilities::atoi( etl::string_view( "000000007" ) ) );
}

TEST( Atoi, EmptyString )
{
    EXPECT_EQ( -1, utilities::atoi( etl::string_view( "" ) ) );  // expect error
}

TEST( Atoi, UnexpectedCharacter )
{
    EXPECT_EQ( -1, utilities::atoi( etl::string_view( "12346Y5789" ) ) );  // expect error
}

TEST( Atoi, Substring )
{
    etl::string_view str = "Gogogogo01234wooooooo";
    EXPECT_EQ( 1234, utilities::atoi( str.substr( 8, 5 ) ) );
}
