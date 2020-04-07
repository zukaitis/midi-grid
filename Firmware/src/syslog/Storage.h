#pragma once

#include "syslog/StorageInterface.h"

#include <etl/array.h>
#include <etl/deque.h>

namespace syslog
{

class Storage : public StorageInterface
{
public:
    Storage();
    ~Storage() override;

    void addEntry( const etl::string_view& entry ) override;
    bool getEntry( etl::string_view* entry, uint16_t index ) override;
    uint16_t getEntryCount() override;

private:

    struct Record
    {
        uint16_t startIndex;
        uint16_t length;
    };

    void copyData( const Record& record, const etl::string_view& entry );
    const Record& createRecord( const etl::string_view& entry );
    void makeSpace( const etl::string_view& entry );

    etl::array<char, 4096> buffer_;
    etl::deque<Record, 128> record_;
};

}  // namespace syslog
