#include "syslog/LogThread.h"
#include "syslog/info.h"

#include "main.hpp"

namespace syslog
{

LogInterface& info = Main::getInstance().getLogContainer().getInfo();

LogContainer::LogContainer( lcd::LcdInterface* lcdInterface ):
    storage_(),
    logThread_( lcdInterface, &storage_ ),
    info_( &logThread_ ),
    error_( &logThread_ )
{
}

LogInterface& LogContainer::getInfo()
{
    return info_;
}

LogInterface& LogContainer::getError()
{
    return error_;
}

}  // namespace syslog
