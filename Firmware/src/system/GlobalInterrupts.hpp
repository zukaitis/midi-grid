#pragma once

#include "system/GlobalInterruptsInterface.h"

namespace mcu {

class GlobalInterrupts : public GlobalInterruptsInterface
{
public:
    GlobalInterrupts() = default;
    virtual ~GlobalInterrupts() = default;

    void disable() override;
    void enable() override;
};

} // namespace
