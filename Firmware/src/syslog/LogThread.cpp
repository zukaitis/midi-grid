#include "syslog/LogThread.h"
#include "lcd/LcdInterface.h"
#include "syslog/StorageInterface.h"

namespace syslog
{

LogThread::LogThread( lcd::LcdInterface* lcd, StorageInterface* storage ):
    Thread( "Log", 256, 3 ),
    lcd_( *lcd ),
    storage_( *storage ),
    timeout_( false ),
    displayTimeMs_( 0 )
{
    Start();
}

LogThread::~LogThread() = default;

void LogThread::Run()
{
    static const uint32_t delayPeriod = 100;

    timeout_.Take(); // wait for semaphore

    while (displayTimeMs_ >= delayPeriod)
    {
        displayTimeMs_ -= delayPeriod;
        Thread::DelayUntil( delayPeriod );
    }
    lcd_.release();
}

void LogThread::append( const etl::string_view& message )
{
    storage_.addEntry( message );
}

void LogThread::display()
{
    lcd_.text().print( "System Log", 0, lcd::Format().justification( lcd::Justification::CENTER ) );

    uint8_t line = 5;
    if (storage_.getEntryCount() < 5)
    {
        line = storage_.getEntryCount();
    }

    uint8_t index = 0;
    while (line > 0)
    {
        etl::string_view entry;
        if (storage_.getEntry( &entry, index ))
        {
            lcd_.text().print( entry, line * 8, lcd::Format() );
        }
        index++;
        line--;
    }
}

void LogThread::displayFor( const uint32_t timeMs )
{
    if (timeMs > displayTimeMs_)
    {
        displayTimeMs_ = timeMs;
        timeout_.Give();
    }
    display();
}

}  // namespace syslog
