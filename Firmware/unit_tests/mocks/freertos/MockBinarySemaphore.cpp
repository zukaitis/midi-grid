#include "freertos/MockBinarySemaphore.h"
#include "freertos/semaphore.hpp"

namespace freertos
{

MockBinarySemaphore& MockBinarySemaphore::getInstance()
{
    static MockBinarySemaphore instance;
    return instance;
}

BinarySemaphore::BinarySemaphore( bool set )
{
}

bool BinarySemaphore::Take( TickType_t Timeout )
{
    return MockBinarySemaphore::getInstance().Take();
}

bool BinarySemaphore::Give()
{
    return MockBinarySemaphore::getInstance().Give();
}

}
