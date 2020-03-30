#pragma once

#include <cstdint>
#include <etl/string_view.h>

namespace log
{

class LogThreadInterface
{
public:
    ~LogThreadInterface() = default;

    virtual void append( const etl::string_view& message ) = 0;
    virtual void display() = 0;
    virtual void displayFor( uint32_t timeMs ) = 0;

};

}  // namespace log
