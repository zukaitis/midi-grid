/*
 * grid_buttons.c
 *
 *  Created on: 2018-01-29
 *      Author: Gedas
 */
#include "grid_buttons/grid_buttons_init.h"
#include "stm32f4xx_hal.h"

static const uint16_t columnSelectValue = { 0xF8DF, 0xF9DF, 0xFADF, 0xFBDF,
                                            0xFCDF, 0xFDDF, 0xFEDF, 0xFFDF,
                                            0xF8EF, 0xF9EF, 0xFAEF, 0xFBEF,
                                            0xFCEF, 0xFDEF, 0xFEEF, 0xFFEF,
                                            0x70FF, 0x71FF, 0x72FF, 0x73FF };

void grid_initialize()
{
    grid_initializeGpio();
}

void grid_setOutput()
{
    GPIOA->ODR = 0xFFEF;
}
