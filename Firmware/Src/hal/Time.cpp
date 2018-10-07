#include "hal/Time.h"
#include "stm32f4xx_hal.h"

namespace hal {

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
