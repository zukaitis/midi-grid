#ifndef LCD_BACKLIGHT_H_
#define LCD_BACKLIGHT_H_

#include <stdint.h>

namespace lcd
{

class Backlight
{
public:
    Backlight();
    ~Backlight();

    void initialize();
    void setIntensity( uint8_t intensity );

    static const uint8_t maximumIntensity = 64;

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    static const uint16_t kOutputBufferSize_ = 32; // 10 bit resolution
    static uint32_t outputBuffer_[kOutputBufferSize_];
};

} // namespace lcd

#endif // LCD_BACKLIGHT_H_
