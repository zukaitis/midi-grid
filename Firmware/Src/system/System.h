#ifndef SYSTEM_H_
#define SYSTEM_H_

namespace mcu
{

class System
{
public:
    System();
    virtual ~System();

    void initialize();
    bool isUsbConnected();
    void resetIntoBootloader();

private:
    void configureNvicPriorities();
    void configureSystemClock();
};

} // namespace hal

#endif /* SYSTEM_H_ */
