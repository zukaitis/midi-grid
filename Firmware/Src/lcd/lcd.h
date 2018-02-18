/*
 * Nokia5110_LCD Library
 *
 * This library is used to control Nokia's 5110 LCD's on STM32 devices.
 * It was built on STM's HAL and intends to offer an easy and fast way to use 5110 using HAL's GPIOs.
 * It's also based on two other 5110 libraries:
 *  1) Tilen Majerle's
 * 	   @website	http://stm32f4-discovery.com
 *	   @link	http://stm32f4-discovery.com/pcd8544-nokia-33105110-lcd-stm32f429-discovery-library/
 * 	   This library was build for STM devices, but it doesn't run on HAL.
 * 	   Tilen also has a great site about STM32.
 *
 *  2) RinkyDinkElectronic's
 * 	   @website http://www.RinkyDinkElectronics.com/
 * 	   This library was built to control 5110 for Arduino.
 *
 * I've studied both libraries and used many things of them. Some functions are exactly the same but
 * there are modified functions too.
 *
 * This library is an unfinished job (but it works!). Some things that might be added in the future:
 *  1) The rest of the functions of both libraries mentioned above
 *  2) ?
 *
 * Feel free to use this library and modify it. :D
 *
 * ----- How to use this library? -----
 * This library was built on HAL for STM32 devices, so HAL is necessary. It's recommended to use it
 * with STM32CubeMX.
 *
 * Steps to use this library:
 * 1) Import this library into your code (!)
 *
 * 2) Configure your pins using set function: LCD_setRST(PORT, PIN)
 * 						   					  LCD_setCE(PORT, PIN)
 * 						   					  LCD_setDC(PORT, PIN)
 * 						   					  LCD_setDIN(PORT, PIN)
 * 						   					  LCD_setCLK(PORT, PIN)
 *
 * 3) Call LCD_init() to initialize the LCD
 * Now the display is initialized and ready to use.
 *
 * Example:
 * 4) Call LCD_print("Hello World", 0, 0)
 *
 * --------------------
 * Author: Caio Rodrigo
 * Github: https://github.com/Zeldax64?tab=repositories
 * --------------------
 */

#ifndef nokia5110_LCD
#define nokia5110_LCD

#include <stdbool.h>
#include "font.h"
#include "stm32f4xx_hal.h"

#define LCD_COMMAND 0
#define LCD_DATA 1

#define LCD_SETYADDR 0x40
#define LCD_SETXADDR 0x80
#define LCD_DISPLAY_BLANK 0x08
#define LCD_DISPLAY_NORMAL 0x0C
#define LCD_DISPLAY_ALL_ON 0x09
#define LCD_DISPLAY_INVERTED 0x0D

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define LCD_SIZE LCD_WIDTH * LCD_HEIGHT / 8

/*
 * @brief LCD parameters
 */
struct LCD_att{
	uint8_t buffer[LCD_SIZE];
	bool inverttext;
};

/*
 * @brief GPIO ports used
 */
struct LCD_GPIO{
	GPIO_TypeDef* RSTPORT;
	uint16_t RSTPIN;

	GPIO_TypeDef* CEPORT;
	uint16_t CEPIN;

	GPIO_TypeDef* DCPORT;
	uint16_t DCPIN;

	GPIO_TypeDef* DINPORT;
	uint16_t DINPIN;

	GPIO_TypeDef* CLKPORT;
	uint16_t CLKPIN;
};

/*----- GPIO Pins -----*/
void LCD_setRST(GPIO_TypeDef* PORT, uint16_t PIN);
void LCD_setCE(GPIO_TypeDef* PORT, uint16_t PIN);
void LCD_setDC(GPIO_TypeDef* PORT, uint16_t PIN);
void LCD_setDIN(GPIO_TypeDef* PORT, uint16_t PIN);
void LCD_setCLK(GPIO_TypeDef* PORT, uint16_t PIN);

/*----- Library Functions -----*/
void LCD_send(uint8_t val);
void LCD_write(uint8_t data, uint8_t mode);
void LCD_init();
void LCD_invert(bool mode);
void LCD_invertText(bool mode);
void LCD_putChar(char c);
void LCD_print(char *str, uint8_t x, uint8_t y);
void LCD_clrScr();
void LCD_goXY(uint8_t x, uint8_t y);

/*----- Draw Functions -----*/
/*
 * These functions draw in a buffer variable. It's necessary to use LCD_refreshScr() or LCD_refreshArea()
 * in order to send data to the LCD.
 */
void LCD_refreshScr();
void LCD_refreshArea(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax);
void LCD_setPixel(uint8_t x, uint8_t y, bool pixel);
void LCD_drawHLine(int x, int y, int l);
void LCD_drawVLine(int x, int y, int l);
void LCD_drawLine(int x1, int y1, int x2, int y2);
void LCD_drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

#endif
