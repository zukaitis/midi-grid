/*
 * gui.c
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */
#include "lcd/Gui.h"
#include "lcd/Lcd.h"

namespace gui
{

Gui::Gui() : lcd( lcd::Lcd::getInstance() )
{}

Gui::~Gui()
{}

void Gui::displayLaunchpad95Mode( launchpad::Launchpad95Mode mode )
{
    lcd.print("              ", 0, 32); // clear that portion of the lcd
    if (launchpad::Launchpad95Mode_UNKNOWN != mode)
    {
        lcd.print(launchpad95ModeString[mode], lcd::WIDTH/2, 32, lcd::Justification_CENTER);
    }
}

void Gui::gui_changeLaunchpad95Submode()
{

}

void Gui::displayUsbLogo()
{
    lcd.displayImage(0, 0, lcd::usbLogo);
}

void Gui::refresh()
{
    lcd.refresh();
}

} // namespace
