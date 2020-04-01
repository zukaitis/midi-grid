#pragma once

#include "lcd/LcdInterface.h"
#include "syslog/Log.h"
#include "syslog/LogThread.h"

namespace syslog
{

class LogContainer
{
public:
    explicit LogContainer( lcd::LcdInterface* lcdInterface );

    LogInterface& getInfo();
    LogInterface& getError();

private:
    LogThread logThread_;

    Log info_;
    Log error_;
};

}  // namespace syslog
