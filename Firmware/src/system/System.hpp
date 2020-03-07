#pragma once

#include <cstdint>

namespace mcu
{

class System
{
public:
    System();
    virtual ~System() = default;

    void initialize();
    bool isUsbConnected();
    void resetIntoBootloader();

private:
    void configureNvicPriorities();
    void configureSystemClock();
};

}  // namespace mcu
