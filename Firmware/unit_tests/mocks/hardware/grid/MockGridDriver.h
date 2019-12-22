#pragma once

#include <gmock/gmock.h>

#include "hardware/grid/InputInterface.h"
#include "hardware/grid/OutputInterface.h"

#include "freertos/thread.hpp"
#include "types/Coordinates.h"

namespace hardware
{
namespace grid
{

class MockGridDriver : public InputInterface, public OutputInterface
{
public:
    virtual ~MockGridDriver() = default;

    MOCK_METHOD( void, addThreadToNotify, (freertos::Thread* const thread), (override) );
    MOCK_METHOD( const InputDebouncingBuffers&, getInputDebouncingBuffers, (), (const, override) );
    MOCK_METHOD( const InputBuffer&, getStableInputBuffer, (), (const, override) );

    MOCK_METHOD( void, setRedOutput, (const Coordinates& coords, const std::uint32_t value), (override) );
    MOCK_METHOD( void, setGreenOutput, (const Coordinates& coords, const std::uint32_t value), (override) );
    MOCK_METHOD( void, setBlueOutput, (const Coordinates& coords, const std::uint32_t value), (override) );

    MOCK_METHOD( void, setAllOff, (), (override) );
};

}

}
