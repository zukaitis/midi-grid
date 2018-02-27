/*
 * gui.h
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */

#ifndef LCD_GUI_H_
#define LCD_GUI_H_

//0:
//melodic seq - 9
//u2-45
//drum seq - 53
//off - 55
//
//1:
//off- 31
//mixer -29
//
//2:
//device c-48
//u1 - 45
//instrument - 37
//off-39
//
//3:
//off- 23
//session - 21

enum GuiMode
{
    GuiMode_LAUNCHPAD95,
    GuiMode_LAUNCHPAD_MK2
};

#endif /* LCD_GUI_H_ */
