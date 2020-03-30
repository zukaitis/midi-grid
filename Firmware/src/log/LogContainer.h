#pragma once

#include "lcd/LcdInterface.h"
#include "log/Log.h"
#include "log/LogThread.h"

namespace log
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

}  // namespace log
