/*
 * gui.h
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */

#ifndef LCD_GUI_H_
#define LCD_GUI_H_

//#include "lcd/Lcd.h"
#include "program/Launchpad.h"

namespace lcd
{
    class Lcd;
}

namespace launchpad
{
    class Launchpad;
}

namespace gui
{

static const char launchpad95ModeString[8][16] = {
        "Session\0",
        "Instrument\0",
        "Device control\0",
        "User 1\0",
        "Drum step seq.\0",
        "Melodic seq.\0",
        "User 2\0",
        "Mixer\0"
};

class Gui
{
public:
    Gui();
    ~Gui();

    void displayLaunchpad95Mode( launchpad::Launchpad95Mode mode );
    void gui_changeLaunchpad95Submode();
    void displayUsbLogo();

    void refresh();

private:
    lcd::Lcd& lcd;
};

} // namespace

#endif /* LCD_GUI_H_ */
