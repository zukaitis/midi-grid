#pragma once

#include <cstdint>

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
    bool enabled;
    uint32_t timeToDisable;
};

class Launchpad;

class LcdGui
{
public:
    LcdGui( Launchpad* launchpad, lcd::LcdInterface* lcd );

    void initialize();
    void refresh();

    void registerMidiInputActivity();
    void registerMidiOutputActivity();
    void displayRotaryControlValues();

    static const int16_t refreshPeriodMs = 250;
private:
    void refreshStatusBar();
    void refreshMode();
    void refreshTimingArea();
    void refreshModeDependentArea();
    void displayClipView();
    void refreshRotaryControlArea();

    void refreshMainArea();
    void refreshTimedItemsStatus();
    void displayLaunchpad95Info();
    void displayClipName();
    void displayDeviceName();
    void displayTrackName();
    void displayMode();
    void displaySubmode();

    Launchpad& launchpad_;
    lcd::LcdInterface& lcd_;

    TimedDisplay usbMidiInputActivityIcon_;
    TimedDisplay usbMidiOutputActivityIcon_;
};

}  // namespace launchpad
}  // namespace application
