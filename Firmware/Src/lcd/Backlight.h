#ifndef LCD_BACKLIGHT_H_
#define LCD_BACKLIGHT_H_

#include <stdint.h>

namespace lcd
{
namespace backlight
{

class Backlight
{
public:
    Backlight();
    ~Backlight();

    void initialize();
    void setIntensity( uint8_t intensity );

    static const uint8_t kNumberOfIntensityLevels = 65;

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    static const uint16_t kOutputBufferSize = 32; // 10 bit resolution
    static uint32_t outputBuffer_[kOutputBufferSize];
};

} // namespace backlight
} // namespace lcd

#endif // LCD_BACKLIGHT_H_
