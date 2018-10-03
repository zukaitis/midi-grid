#ifndef SYSTEM_H_
#define SYSTEM_H_

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

#endif /* SYSTEM_H_ */
