#ifndef SYSTEM_GLOBALINTERRUPTS_HPP_
#define SYSTEM_GLOBALINTERRUPTS_HPP_

namespace mcu {

class GlobalInterrupts
{
public:
    GlobalInterrupts();
    virtual ~GlobalInterrupts();

    void disable();
    void enable();
};

} // namespace hal

#endif // SYSTEM_GLOBALINTERRUPTS_HPP_
