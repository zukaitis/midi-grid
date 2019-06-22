#include "system/GlobalInterrupts.hpp"

#include "stm32f4xx_hal.h"

namespace mcu
{

GlobalInterrupts::GlobalInterrupts()
{
}

GlobalInterrupts::~GlobalInterrupts()
{
}

void GlobalInterrupts::disable()
{
    __disable_irq();
}

void GlobalInterrupts::enable()
{
    __enable_irq();
}

} // namespace hal
