#pragma once

#include "hardware/grid/InputInterface.h"
#include "hardware/grid/OutputInterface.h"

#include <etl/array.h>
#include <etl/vector.h>

namespace hardware
{
namespace grid
{

class GridDriver : public InputInterface, public OutputInterface
{
public:
    GridDriver();

    static void notifyThreads();

    void addThreadToNotify( freertos::Thread* const thread ) override;
    const InputDebouncingBuffers& getInput() const override;

    void setRedOutput( const Coordinates& coords, const std::uint32_t value ) override;
    void setGreenOutput( const Coordinates& coords, const std::uint32_t value ) override;
    void setBlueOutput( const Coordinates& coords, const std::uint32_t value ) override;

    void setAllOff() override;

    void initialize() const;
    void start() const;

    // TODO: remove this legacy stuff
    uint8_t getRotaryEncodersInput( const uint8_t encoder, const uint8_t timeStep ) const;

private:
    void initializeBaseTimer() const;
    void initializeDma() const;
    void initializeGpio() const;
    void initializePwmGpio() const;
    void initializePwmTimers() const;

    static InputDebouncingBuffers input_;
    static etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> redOutput_;
    static etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> greenOutput_;
    static etl::array<etl::array<uint32_t, numberOfRows>, numberOfColumns> blueOutput_;

    static etl::vector<freertos::Thread*, 7> threadToNotify_;
};

}
}
