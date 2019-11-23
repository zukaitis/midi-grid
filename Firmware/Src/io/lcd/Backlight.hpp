#ifndef LCD_BACKLIGHT_HPP_
#define LCD_BACKLIGHT_HPP_

#include <stdint.h>

#include <freertos/thread.hpp>
#include <freertos/semaphore.hpp>

namespace lcd
{

class Backlight : private freertos::Thread
{
public:
    Backlight();

    void initialize();
    void setIntensity( uint8_t intensity );

    static const uint8_t maximumIntensity = 64;

private:
    void Run();

    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    void setMomentaryIntensity( uint8_t intensity );

    static const uint16_t kOutputBufferSize_ = 32; // 10 bit resolution
    static uint32_t outputBuffer_[kOutputBufferSize_];

    uint8_t appointedIntensity_;
    uint8_t currentIntensity_;

    freertos::BinarySemaphore appointedIntensityChanged_;
};

} // namespace lcd

#endif // LCD_BACKLIGHT_HPP_
