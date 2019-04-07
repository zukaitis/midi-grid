#ifndef APPLICATION_INTERNAL_MENU_H_
#define APPLICATION_INTERNAL_MENU_H_

#include "thread.hpp"

#include <functional>

namespace grid
{
    class Grid;
    class AdditionalButtons;
}

namespace lcd
{
    class Gui;
}

namespace mcu
{
    class System;
}

namespace internal_menu {

class AdditionalButtonInputHandler: public freertos::Thread
{
public:
    AdditionalButtonInputHandler( grid::AdditionalButtons& additionalButtons, std::function<void()> stopApplicationCallback );

    virtual void Run();
private:
    grid::AdditionalButtons& additionalButtons_;
    std::function<void()> stopApplication_;
};

class GridInputHandler: public freertos::Thread
{
public:
    GridInputHandler( grid::Grid& grid, mcu::System& system );

    virtual void Run();
private:
    grid::Grid& grid_;
    mcu::System& system_;
};

class InternalMenu
{
public:
    InternalMenu( grid::Grid& grid, grid::AdditionalButtons& additionalButtons, lcd::Gui& gui, mcu::System& system, std::function<void(uint8_t)> switchApplicationCallback );

    void enable();
    void disable();

    void stopApplicationCallback();

private:
    grid::Grid& grid_;
    lcd::Gui& gui_;

    AdditionalButtonInputHandler additionalButtonInputHandler_;
    GridInputHandler gridInputHandler_;

    std::function<void(const uint8_t)> switchApplication_;
};

} // namespace

#endif // APPLICATION_INTERNAL_MENU_H_