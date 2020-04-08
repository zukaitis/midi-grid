#include "syslog/Storage.h"

#include <etl/cstring.h>

namespace syslog
{

Storage::Storage():
    buffer_(),
    record_()
{
}

Storage::~Storage() = default;

void Storage::addEntry( const etl::string_view& entry )
{
    makeSpace( entry );
    const Record& record = createRecord( entry );
    copyData( record, entry );
}

bool Storage::getEntry( etl::string_view* const entry, const uint16_t index )
{
    bool entryAvailable = false;

    if (index < record_.size())
    {
        const uint16_t startIndex = record_.at(index).startIndex;
        *entry = etl::string_view( &buffer_.at(startIndex), record_.at(index).length );

        entryAvailable = true;
    }

    return entryAvailable;
}

uint16_t Storage::getEntryCount()
{
    return record_.size();
}

void Storage::copyData( const Record& record, const etl::string_view& entry )
{
    uint16_t destinationIndex = record.startIndex;
    for (char c : entry)
    {
        buffer_.at( destinationIndex ) = c;
        destinationIndex++;
    }
}

const Storage::Record& Storage::createRecord( const etl::string_view& entry )
{
    uint16_t newStartIndex = record_.front().startIndex + record_.front().length;
    if ((newStartIndex + entry.length()) > buffer_.size())
    {
        newStartIndex = 0;
    }

    const Record newRecord = { newStartIndex, static_cast<uint16_t>(entry.length()) };
    record_.emplace_front( newRecord );

    return record_.front();
}

void Storage::makeSpace( const etl::string_view& entry )
{
    if (false == record_.empty())
    {
        if (record_.full())
        {
            record_.pop_back(); // if entry limit is reached, free one up
        }

        uint16_t newStartIndex = record_.front().startIndex + record_.front().length;
        if ((newStartIndex + entry.length()) > buffer_.size())
        {
            while (newStartIndex <= record_.back().startIndex)
            {
                // remove all records from the end of the buffer
                record_.pop_back();
            }
            newStartIndex = 0;
        }

        if (newStartIndex <= record_.back().startIndex)
        {
            while ((newStartIndex + entry.length()) > record_.back().startIndex)
            {
                // keep removing oldest records, until there is enough space for a new one
                record_.pop_back();
            }
        }
    }
}

}  // namespace syslog
