#pragma once

#include "syslog/LogThreadInterface.h"

#include "lcd/LcdInterface.h"
#include "syslog/StorageInterface.h"
#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>

namespace syslog
{

class LogThread : public LogThreadInterface, private freertos::Thread
{
public:
    LogThread( lcd::LcdInterface* lcd, StorageInterface* storage );
    ~LogThread() override;

    void append( const etl::string_view& message ) override;
    void display() override;
    void displayFor( uint32_t timeMs ) override;

    void Run() override;

private:
    lcd::LcdInterface& lcd_;
    StorageInterface& storage_;

    freertos::BinarySemaphore timeout_;
    uint32_t displayTimeMs_;
};

}  // namespace syslog
