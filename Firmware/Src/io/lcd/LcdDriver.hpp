#ifndef LCD_LCDCONTROL_HPP_
#define LCD_LCDCONTROL_HPP_

#include <stdint.h>

namespace lcd
{

class LcdDriver
{
public:
    LcdDriver();
    ~LcdDriver();

    void initialize();
    void transmit( uint8_t* const buffer );

    static const uint16_t bufferSize = 504;
private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    void resetController();
    void setCursor( const uint8_t x, const uint8_t y );
    void writeCommand( const uint8_t command );
};

} // namespace lcd

#endif // LCD_LCDCONTROL_HPP_
