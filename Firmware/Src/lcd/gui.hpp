/*
 * gui.h
 *
 *  Created on: 2018-02-27
 *      Author: Gedas
 */

#ifndef LCD_GUI_H_
#define LCD_GUI_H_

class Gui {
public:
    Gui& getInstance()
    {
        static Gui instance;
        return instance;
    }

    enum GuiMode
    {
        GuiMode_LAUNCHPAD95,
        GuiMode_LAUNCHPAD_MK2
    };

    void gui_changeLaunchpadMode();
    void gui_changeLaunchpad95Submode();

private:
    GuiMode guiMode; // = GuiMode_LAUNCHPAD95;

};

#endif /* LCD_GUI_H_ */
