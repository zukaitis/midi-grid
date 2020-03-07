#include "system/GlobalInterrupts.hpp"

#include "freertos/critical.hpp"

namespace mcu
{

void GlobalInterrupts::enable()
{
    freertos::CriticalSection::EnterFromISR();
}

void GlobalInterrupts::disable()
{
    BaseType_t unused;
    freertos::CriticalSection::ExitFromISR( unused );
}

}  // namespace mcu
