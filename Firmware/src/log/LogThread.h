#pragma once

#include "log/LogThreadInterface.h"

#include "lcd/LcdInterface.h"
#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>

namespace log
{

class LogThread : public LogThreadInterface, private freertos::Thread
{
public:
    explicit LogThread( lcd::LcdInterface* lcd );
    ~LogThread() override;

    void append( const etl::string_view& message ) override;
    void display() override;
    void displayFor( uint32_t timeMs ) override;

    void Run() override;

private:
    lcd::LcdInterface& lcd_;

    uint8_t lineNumber_;
};

}  // namespace log
