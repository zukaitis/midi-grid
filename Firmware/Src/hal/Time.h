#ifndef SYSTEM_TIME_H_
#define SYSTEM_TIME_H_

#include <stdint.h>

namespace hal {

class Time
{
public:
    Time();
    virtual ~Time();

    uint32_t getSystemTick();
};

} // namespace hal

#endif /* SYSTEM_TIME_H_ */
