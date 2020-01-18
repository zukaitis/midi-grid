#ifndef APPLICATION_LAUNCHPAD_LCD_GUI_HPP_
#define APPLICATION_LAUNCHPAD_LCD_GUI_HPP_

#include <stdint.h>

namespace lcd
{
    class LcdInterface;
}

namespace application
{
namespace launchpad
{

struct TimedDisplay
{
    bool isOn;
    uint32_t timeToDisable;
};

class Launchpad;

class LcdGui
{
public:
    LcdGui( Launchpad& launchpad, lcd::LcdInterface& lcd );

    void initialize();
    void refresh();

    void registerMidiInputActivity();
    void registerMidiOutputActivity();
    void displayRotaryControlValues();

    static const int16_t refreshPeriodMs = 250;
private:
    void refreshStatusBar();
    void refreshMainArea();
    void displayLaunchpad95Info();
    void displayClipName();
    void displayDeviceName();
    void displayTrackName();
    void displayMode();
    void displaySubmode();
    void displayStatus();
    void displayTimingStatus();

    Launchpad& launchpad_;
    lcd::LcdInterface& lcd_;

    TimedDisplay midiInputActivityIcon_;
    TimedDisplay midiOutputActivityIcon_;
    TimedDisplay rotaryControlValues_;
};


}
} // namespace


#endif // APPLICATION_LAUNCHPAD_LCD_GUI_HPP_
