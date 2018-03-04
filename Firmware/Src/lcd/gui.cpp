/*
 * gui.c
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */
#include "lcd/gui.hpp"
#include "lcd/lcd.h"
#include "grid_buttons/grid_buttons.h"
#include "program/launchpad.h"

#if 0
void Gui::gui_changeLaunchpadMode()
{
    if (GuiMode_LAUNCHPAD95 == guiMode)
    {
        enum Launchpad95Mode mode = launchpad_getLaunchpad95Mode();
        lcd_print("              ", 0, 32); // clear that portion of the lcd
        if (Launchpad95Mode_UNKNOWN != mode)
        {
            //lcd_printJustified(launchpad95ModeString[mode], LCD_WIDTH/2, 32, Justification_CENTER);
        }
    }
}

void Gui::gui_changeLaunchpad95Submode()
{

}
#endif
