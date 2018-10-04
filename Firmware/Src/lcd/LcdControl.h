#ifndef LCD_LCDCONTROL_H_
#define LCD_LCDCONTROL_H_

namespace lcd
{
namespace lcd_control
{

static const uint16_t kBufferSize = 504;

class LcdControl
{
public:
    LcdControl();
    ~LcdControl();

    void initialize();
    void transmit( uint8_t* const buffer );

private:
    void initializeDma();
    void initializeGpio();
    void initializeSpi();

    void resetController();
    void setCursor( const uint8_t x, const uint8_t y );
    void writeCommand( const uint8_t command );
};

} // namespace lcd_control
} // namespace lcd

#endif // LCD_LCDCONTROL_H_
