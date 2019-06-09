#ifndef APPLICATION_STARTUP_H_
#define APPLICATION_STARTUP_H_

#include "application/Application.h"

namespace grid
{
    class GridDriver;
}

namespace lcd
{
    class Gui;
    class Lcd;
}

namespace mcu
{
    class System;
}

namespace application
{

class Startup : public Application
{
public:
    Startup( ApplicationController& applicationController, grid::GridDriver& gridDriver, lcd::Gui& gui, lcd::Lcd& lcd, mcu::System& system );

private:
    void run( ApplicationThread& thread );

    grid::GridDriver& gridDriver_;
    lcd::Gui& gui_;
    lcd::Lcd& lcd_;
    mcu::System& system_;
};

} // namespace

#endif // APPLICATION_STARTUP_H_