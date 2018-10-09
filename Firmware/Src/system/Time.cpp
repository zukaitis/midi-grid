#include "system/Time.h"
#include "stm32f4xx_hal.h"

namespace mcu {

Time::Time()
{
}

Time::~Time()
{
}

uint32_t Time::getSystemTick()
{
    return HAL_GetTick();
}

} // namespace hal
