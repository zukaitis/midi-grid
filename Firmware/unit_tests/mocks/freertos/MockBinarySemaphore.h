#pragma once

#include "freertos/semaphore.hpp"
#include "freertos/ticks.hpp"

#include <gmock/gmock.h>

namespace freertos
{

class MockBinarySemaphore
{
public:
    static MockBinarySemaphore& getInstance();

    MOCK_METHOD( bool, Take, () );
    MOCK_METHOD( bool, Give, () );

private:
    MockBinarySemaphore() = default;
    virtual ~MockBinarySemaphore() = default;
};

}
