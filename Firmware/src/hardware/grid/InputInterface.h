#pragma once

#include "hardware/grid/Constants.h"
#include <etl/array.h>

namespace freertos
{
    class Thread;
    class BinarySemaphore;
}

namespace hardware
{
namespace grid
{

const uint8_t numberOfInputBuffers = 2;

typedef etl::array<std::uint32_t, numberOfColumns> InputBuffer;
typedef etl::array<InputBuffer, numberOfInputBuffers> InputDebouncingBuffers;

class InputInterface
{
public:
    virtual ~InputInterface() = default;

    virtual void addThreadToNotify( freertos::Thread* thread ) = 0;
    virtual void addSemaphoreToGive( freertos::BinarySemaphore* semaphore ) = 0;
    virtual const InputDebouncingBuffers& getInputDebouncingBuffers() const = 0;
    virtual const InputBuffer& getStableInputBuffer() const = 0;
};

}  // namespace grid
}  // namespace hardware
