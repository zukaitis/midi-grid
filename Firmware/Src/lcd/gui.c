/*
 * gui.c
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */
#include "lcd/gui.h"
#include "lcd/lcd.h"
#include "grid_buttons/grid_buttons.h"

static enum GuiMode guiMode = GuiMode_LAUNCHPAD95;

void gui_changeLaunchpadMode()
{
    if (GuiMode_LAUNCHPAD95 == guiMode)
    {
        do
        {
            //struct Colour ledColour = grid_getColour(9, 3); // session mode led

//            if ({3, 2, 2} == ledColour)
//            {
//
//            }

        } while (0);
    }
}

void gui_changeLaunchpadSubmode()
{

}
