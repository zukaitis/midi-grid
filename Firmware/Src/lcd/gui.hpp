/*
 * gui.h
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */

#ifndef LCD_GUI_H_
#define LCD_GUI_H_

#include "lcd/lcd.hpp"

class Gui {
public:


    enum GuiMode
    {
        GuiMode_LAUNCHPAD95,
        GuiMode_LAUNCHPAD_MK2
    };

    void gui_changeLaunchpadMode();
    void gui_changeLaunchpad95Submode();

    static Gui& getInstance()
    {
        static Gui instance;
        return instance;
    }

    ~Gui();
private:

    Gui();

    const char launchpad95ModeString[8][16] = {
            "Session\0",
            "Instrument\0",
            "Device control\0",
            "User 1\0",
            "Drum step seq.\0",
            "Melodic seq.\0",
            "User 2\0",
            "Mixer\0"
    };

    Lcd& lcd;
    GuiMode guiMode;

};

#endif /* LCD_GUI_H_ */
