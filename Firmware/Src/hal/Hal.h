#ifndef SYSTEM_H_
#define SYSTEM_H_

namespace hal
{

class Hal
{
public:
    Hal();
    virtual ~Hal();

    void initialize();
    bool isUsbConnected();
    void resetIntoBootloader();

private:
    void configureNvicPriorities();
    void configureSystemClock();
};

} // namespace hal

#endif /* SYSTEM_H_ */
