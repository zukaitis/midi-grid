#pragma once

#include "lcd/backlight/BacklightInterface.h"

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
    Backlight( hardware::lcd::BacklightDriverInterface* driver );

    void initialize() override;
    void setIntensity( uint8_t intensity ) override;

private:
    void Run() override;

    hardware::lcd::BacklightDriverInterface& driver_;

    uint8_t appointedIntensity_;
    uint8_t currentIntensity_;

    freertos::BinarySemaphore appointedIntensityChanged_;
};

} // namespace lcd
