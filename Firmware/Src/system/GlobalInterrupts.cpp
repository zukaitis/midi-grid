#include "system/GlobalInterrupts.h"

#include "stm32f4xx_hal.h"

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


