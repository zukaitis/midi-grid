#include "lcd/Lcd.hpp"

#include <stm32f411xe.h> // ?
#include "stm32f4xx_hal.h"
//#include "sys/_stdint.h"
#include <stdint.h>
#include <string.h>

/*----- Library Functions -----*/

Lcd::Lcd(): updateRequired( false )
{
}

Lcd::~Lcd()
{
}

void Lcd::initialize()
{
    lcdControl.initialize();
    clear();
}

void Lcd::clear()
{
    memset(&lcdBuffer[0][0], 0x00, lcdControl.LCD_BUFFER_SIZE);
    updateRequired = true;
}

void Lcd::refresh()
{
    static uint32_t refreshCheckTime = 0;
    if (HAL_GetTick() >= refreshCheckTime)
    {
        if (updateRequired)
        {
            lcdControl.update(&lcdBuffer[0][0]);
            updateRequired = false;
        }
        refreshCheckTime += 10; // check every 10ms
    }
}



void Lcd::putChar(const uint8_t x, const uint8_t y, const char c)
{
    for(uint8_t i = 0; i < 6; i++)
    {
        if ((x+i) >= WIDTH)
        {
            break;
        }
        else
        {
            lcdBuffer[y/8][x+i] &= ~(0xFF << (y % 8));
            lcdBuffer[y/8][x+i] |= ASCII[c-0x20][i] << (y % 8);

            if ((y < (HEIGHT - FONT_HEIGHT)) && (0 != (y % 8)))
            {
                lcdBuffer[y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                lcdBuffer[y/8+1][x+i] |= ASCII[c-0x20][i] >> (8 - y % 8);
            }
        }
    }
}

/*
 * @brief Print a string on the LCD
 * @param x: starting point on the x-axis (column)
 * @param y: starting point on the y-axis (line)
 */
void Lcd::print(const char *string, uint8_t x, const uint8_t y)
{
    if ((x < WIDTH)&&(y < HEIGHT))
    {
        while(*string)
        {
            putChar(x, y, *string++);
            x += 6;
        }
    }
    updateRequired = true;
}

void Lcd::print(const char *string, const uint8_t x, const uint8_t y, const Justification justification)
{
    uint8_t textWidth = strlen(string) * FONT_WIDTH;

    if (Justification_RIGHT ==  justification)
    {
        if (textWidth < x)
        {
            print(string, (x - textWidth), y);
        }
    }
    else if (Justification_CENTER ==  justification)
    {
        textWidth = textWidth / 2;
        if ((textWidth <= x) && (textWidth <= (WIDTH - x)))
        {
            print(string, (x - textWidth), y);
        }
    }
    else // (Justification_LEFT ==  justification)
    {
        if (textWidth < (WIDTH - x))
        {
            print(string, x, y);
        }
    }
}

