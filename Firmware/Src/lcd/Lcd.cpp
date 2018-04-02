#include "lcd/Lcd.h"

#include "stm32f4xx_hal.h"

#include <stdint.h>
#include <string.h>
#include <math.h>

namespace lcd
{

Lcd::Lcd()
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
        refreshCheckTime = HAL_GetTick() + 10; // check every 10ms
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
    if (y < HEIGHT) // width is checked in putChar
    {
        while(*string)
        {
            putChar(x, y, *string++);
            x += 6;
        }
    }
    updateRequired = true;
}

void Lcd::printNumberInBigDigits(const uint16_t number, const uint8_t x, const uint8_t y, const Justification justification)
{
    uint8_t numberOfDigits = 5; // 5 digit numbers max
    uint16_t divisor = 10000;
    while (divisor > 1)
    {
        if (0 != (number / divisor))
        {
            break;
        }
        --numberOfDigits;
        divisor /= 10;
    }

    uint8_t textWidth = numberOfDigits * digitBig[0].width;

    switch (justification)
    {
        case Justification_RIGHT:
            if (textWidth < x)
            {
                printNumberInBigDigits( number, (x - textWidth), y, numberOfDigits );
            }
            break;
        case Justification_CENTER:
            textWidth = textWidth / 2;
            if ((textWidth <= x) && (textWidth <= (WIDTH - x)))
            {
                printNumberInBigDigits( number, (x - textWidth), y, numberOfDigits );
            }
            break;
        case Justification_LEFT:
        default:
            if (textWidth < (WIDTH - x))
            {
                printNumberInBigDigits(number, x, y, numberOfDigits);
            }
            break;
    }
}

void Lcd::printNumberInBigDigits( uint16_t number, uint8_t x, const uint8_t y, const uint8_t numberOfDigits )
{
    uint16_t divisor = pow(10, numberOfDigits);
    while (divisor > 1)
    {
        number %= divisor;
        divisor /= 10;
        displayImage(x, y, digitBig[number/divisor]);
        x += digitBig[0].width;
    }
}

void Lcd::print(const char *string, const uint8_t x, const uint8_t y, const Justification justification)
{
    uint8_t textWidth = strlen(string) * FONT_WIDTH;

    switch (justification)
    {
        case Justification_RIGHT:
            if (textWidth < x)
            {
                print(string, (x - textWidth), y);
            }
            break;
        case Justification_CENTER:
            textWidth = textWidth / 2;
            if ((textWidth <= x) && (textWidth <= (WIDTH - x)))
            {
                print(string, (x - textWidth), y);
            }
            break;
        case Justification_LEFT:
        default:
            if (textWidth < (WIDTH - x))
            {
                print(string, x, y);
            }
            break;
    }
}

void Lcd::displayImage(const uint8_t x, const uint8_t y, const Image image)
{
    for (uint8_t j = 0; j < (image.height/8); j++)
    {
        for(uint8_t i = 0; i < image.width; i++)
        {
            if ((x+i) >= WIDTH)
            {
                break;
            }
            else
            {
                lcdBuffer[j+y/8][x+i] &= ~(0xFF << (y % 8));
                lcdBuffer[j+y/8][x+i] |= image.image[j*image.width + i] << (y % 8);

                if (((j*8 + y) < (HEIGHT - 8)) && (0 != (y % 8)))
                {
                    lcdBuffer[j+y/8+1][x+i] &= ~(0xFF >> (8 - y % 8));
                    lcdBuffer[j+y/8+1][x+i] |= image.image[j*image.width + i] >> (8 - y % 8);
                }
            }
        }
    }
    updateRequired = true;
}

void Lcd::clearArea(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2)
{
    for (uint8_t j = 0; j <= ((y2-y1)/8); j++)
    {
        for(uint8_t i = 0; i < (x2-x1+1); i++)
        {
            if ((x1+i) >= WIDTH)
            {
                break;
            }
            else
            {
                lcdBuffer[j+y1/8][x1+i] &= ~(0xFF << (y1 % 8));
                if (((j*8 + y1) < (HEIGHT - 8)) && (0 != (y1 % 8)))
                {
                    lcdBuffer[j+y1/8+1][x1+i] &= ~(0xFF >> (8 - y1 % 8));
                }
            }
        }
    }
    updateRequired = true;
}

} // namespace
