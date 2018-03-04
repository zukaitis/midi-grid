/*
 * gui.c
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */
#include "lcd/gui.hpp"
#include "lcd/lcd.hpp"
#include "grid_buttons/grid_buttons.hpp"
#include "program/launchpad.hpp"

Gui::Gui() :
        lcd( Lcd::getInstance() ),
        guiMode( GuiMode_LAUNCHPAD95 )
{}

Gui::~Gui()
{}

void Gui::gui_changeLaunchpadMode()
{
    if (GuiMode_LAUNCHPAD95 == guiMode)
    {
        enum Launchpad95Mode mode = launchpad_getLaunchpad95Mode();
        lcd.print("              ", 0, 32); // clear that portion of the lcd
        if (Launchpad95Mode_UNKNOWN != mode)
        {
            lcd.print(launchpad95ModeString[mode], lcd.WIDTH/2, 32, lcd.Justification_CENTER);
        }
    }
}

void Gui::gui_changeLaunchpad95Submode()
{

}
