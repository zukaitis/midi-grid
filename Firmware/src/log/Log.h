#pragma once

#include "log/LogInterface.h"
#include "log/LogThreadInterface.h"

#include <etl/string_view.h>

namespace log
{

class Log : public LogInterface
{
public:
    explicit Log( LogThreadInterface* log );
    ~Log() override;

    void operator<<( const etl::string_view& message ) const override;
    void operator<<( const char* message ) const override;

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

}  // namespace log
