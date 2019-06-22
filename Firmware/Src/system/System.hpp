#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

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

#endif // SYSTEM_HPP_
