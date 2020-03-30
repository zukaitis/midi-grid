#include "log/LogThread.h"

namespace log
{

LogThread::LogThread( lcd::LcdInterface* lcd ):
    Thread( "Log", 256, 3 ),
    lcd_( *lcd ),
    lineNumber_( 0 )
{
}

LogThread::~LogThread() = default;

void LogThread::append( const etl::string_view& message )
{
    lcd_.clearArea( 0, lineNumber_ * 8, 83, lineNumber_ * 8 + 7 );
    lcd_.print( message.begin(), 0, lineNumber_ * 8 );

    lineNumber_ = (lineNumber_ + 1) % 6;
}

void LogThread::display()
{
}

void LogThread::displayFor( const uint32_t timeMs )
{
}

void LogThread::Run()
{
}


}  // namespace log
