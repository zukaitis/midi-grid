#pragma once

namespace mcu {

class GlobalInterruptsInterface
{
public:
    virtual ~GlobalInterruptsInterface() = default;

    virtual void enable() = 0;
    virtual void disable() = 0;
};

}  // namespace mcu
