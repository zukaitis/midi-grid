#pragma once

#include <cstdint>
#include <etl/string_view.h>

namespace syslog
{

class LogInterface
{
public:
    virtual ~LogInterface() = default;

    virtual void operator<<( const etl::string_view& message ) const = 0;
    virtual void operator<<( const char* message ) const = 0;
    virtual void operator<<( uint32_t value ) const = 0;

    virtual void setDisplayTimeout( uint32_t timeoutMs ) = 0;
    virtual void disableDisplayTimeout() = 0;
    virtual void disableDisplay() = 0;
};

}
