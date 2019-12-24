#include "system/GlobalInterrupts.hpp"

#include "stm32f4xx_hal.h"

namespace mcu
{

void GlobalInterrupts::enable()
{
    __enable_irq();
}

void GlobalInterrupts::disable()
{
    __disable_irq();
}

} // namespace hal
