#pragma once

#include "io/lcd/BacklightInterface.h"

#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>

namespace hardware
{
    namespace lcd
    {
        class BacklightDriverInterface;
    }
}

namespace lcd
{

class Backlight : private freertos::Thread, public BacklightInterface
{
public:
    Backlight( hardware::lcd::BacklightDriverInterface& driver );

    void initialize() override;
    void setIntensity( const uint8_t intensity ) override;

    uint8_t getMaximumIntensity() const override;

private:
    void Run();

    hardware::lcd::BacklightDriverInterface& driver_;

    uint8_t appointedIntensity_;
    uint8_t currentIntensity_;

    freertos::BinarySemaphore appointedIntensityChanged_;
};

} // namespace lcd
