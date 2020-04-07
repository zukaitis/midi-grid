#pragma once

#include "syslog/LogInterface.h"
#include "syslog/LogThreadInterface.h"

#include <etl/string_view.h>

namespace syslog
{

class Log : public LogInterface
{
public:
    explicit Log( LogThreadInterface* logThread );
    ~Log() override;

    void operator<<( const etl::string_view& message ) const override;
    void operator<<( const char* message ) const override;
    void operator<<( uint32_t value ) const override;

    void setDisplayTimeout( uint32_t timeoutMs ) override;
    void disableDisplayTimeout() override;
    void disableDisplay() override;
    
private:
    void display() const;

    LogThreadInterface& logThread_;

    enum class DisplayMode
    {
        OFF,
        TIMED,
        UNTIMED
    };

    DisplayMode mode_;
    uint32_t timeout_;
};

}  // namespace syslog
