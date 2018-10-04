#ifndef SYSTEM_GLOBALINTERRUPTS_H_
#define SYSTEM_GLOBALINTERRUPTS_H_

class GlobalInterrupts
{
public:
    GlobalInterrupts();
    virtual ~GlobalInterrupts();

    void disable();
    void enable();
};

#endif /* SYSTEM_GLOBALINTERRUPTS_H_ */
