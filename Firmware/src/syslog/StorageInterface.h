#pragma once

#include <cstdint>

#include <etl/string_view.h>

namespace syslog
{

class StorageInterface
{
public:
    virtual ~StorageInterface() = default;

    virtual void addEntry( const etl::string_view& entry ) = 0;
    virtual bool getEntry( etl::string_view* entry, uint16_t index ) = 0;

};

}  // namespace syslog
