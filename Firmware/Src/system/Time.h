#ifndef SYSTEM_TIME_H_
#define SYSTEM_TIME_H_

#include <stdint.h>

class Time
{
public:
    Time();
    virtual ~Time();

    uint32_t getSystemTick();
};

#endif /* SYSTEM_TIME_H_ */
