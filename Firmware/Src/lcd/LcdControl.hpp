/*
 * LcdConfiguration.hpp
 *
 *  Created on: 2018-03-04
 *      Author: Gedas
 */

#ifndef LCD_LCDCONTROL_HPP_
#define LCD_LCDCONTROL_HPP_

#include <stdint.h>

class LcdControl
{
public:

#if 0
    static LcdControl& getInstance()
    {
        static LcdControl instance;
        return instance;
    }
#endif
    LcdControl();
    ~LcdControl();
protected:

    void initializeControl();
    void update(uint8_t* buffer);

    const uint16_t LCD_BUFFER_SIZE = 504;

private:



    void resetController();
    void writeCommand( const uint8_t command );

    void initializeBacklightTimer();

    void initializeDma();

    void setCursor( const uint8_t x, const uint8_t y );
    void initializeGpio();

    void initializeSpi();
    void LCD_init();
    void initializeBacklightPwm();

};



#endif /* LCD_LCDCONTROL_HPP_ */
