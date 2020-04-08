#include <gtest/gtest.h>
#include "etl/string_view.h"
#include "syslog/Storage.h"

int main( int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}

TEST( Storage, Constructor )
{
    const syslog::Storage storage;
    SUCCEED();
}

TEST( Storage, getEntryCount )
{
    syslog::Storage storage;
    EXPECT_EQ( 0, storage.getEntryCount() );
    
    storage.addEntry( "Test data, hope this works");
    EXPECT_EQ( 1, storage.getEntryCount() );

    storage.addEntry( "On the other hand, I would rather find some issues, by using this test, instead of just getting it to pass");
    EXPECT_EQ( 2, storage.getEntryCount() );

    storage.addEntry( "Few more entries");
    storage.addEntry( "So I would get total of");
    storage.addEntry( "5 (FIVE)");
    EXPECT_EQ( 5, storage.getEntryCount() );
}

TEST( Storage, CheckConsistencyOfEntry )
{
    syslog::Storage storage;
    const char* testString = "TestString1234567890!@#$%^&*()";

    storage.addEntry( testString );
    etl::string_view entry;
    storage.getEntry( &entry, 0 );
    EXPECT_STREQ( testString, &entry.front() );
}

TEST( Storage, FillBuffer )
{
    syslog::Storage storage;
    const char* testString = "1234567890123456789012345678901234567890123456789012345678901234";

    for (uint16_t i = 0; i < 64; i++)
    {
        storage.addEntry( testString );
    }
    EXPECT_EQ( 64, storage.getEntryCount() ); // all entries should still fit inside the buffer

    storage.addEntry( testString );
    EXPECT_EQ( 64, storage.getEntryCount() ); // no more space inside the buffer, oldest record is removed
}

TEST( Storage, FillRecordArray )
{
    syslog::Storage storage;
    const char* testString = "1234567890";

    for (uint16_t i = 0; i < 128; i++)
    {
        storage.addEntry( testString );
    }
    EXPECT_EQ( 128, storage.getEntryCount() ); // all entries should still fit inside the record array

    storage.addEntry( testString );
    EXPECT_EQ( 128, storage.getEntryCount() ); // no more space inside the record array, oldest record is removed
}
