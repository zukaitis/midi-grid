#include "syslog/Log.h"

#include <etl/cstring.h>
#include <etl/to_string.h>

namespace syslog
{

Log::Log( LogThreadInterface* logThread ):
    logThread_( *logThread ),
    mode_( DisplayMode::TIMED ),
    timeout_( 2000U )
{
}

Log::~Log() = default;

void Log::operator<<( const etl::string_view& message ) const
{
    logThread_.append( message );
    display();
}

void Log::operator<<( const char* message ) const
{
    logThread_.append( message );
    display();
}

void Log::operator<<( const uint32_t value ) const
{
    etl::string<16> valueString = "0x";
    etl::to_string( value, valueString, etl::format_spec().hex(), true );
    logThread_.append( valueString );
    display();
}

void Log::setDisplayTimeout( uint32_t timeoutMs )
{
    mode_ = DisplayMode::TIMED;
    timeout_ = timeoutMs;
}

void Log::disableDisplayTimeout()
{
    mode_ = DisplayMode::UNTIMED;
}

void Log::disableDisplay()
{
    mode_ = DisplayMode::OFF;
}

void Log::display() const
{
    switch (mode_)
    {
        case DisplayMode::TIMED:
            logThread_.displayFor( timeout_ );
            break;
        case DisplayMode::UNTIMED:
            logThread_.display();
            break;
        case DisplayMode::OFF:
        default:
            break;
    }
}

}  // namespace syslog
