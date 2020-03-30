#include "log/Log.h"

namespace log
{

Log::Log( LogThreadInterface* logThread ):
    logThread_( *logThread ),
    mode_( DisplayMode::UNTIMED ),
    timeout_( 0 )
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

void Log::setDisplayTimeout( uint32_t timeoutMs )
{

}

void Log::disableDisplayTimeout()
{

}

void Log::disableDisplay()
{

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

}  // namespace log
