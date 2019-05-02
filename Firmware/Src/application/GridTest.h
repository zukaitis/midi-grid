#ifndef APPLICATION_GRID_TEST_H_
#define APPLICATION_GRID_TEST_H_

#include "application/Application.h"
#include "Types.h"
#include "thread.hpp"

namespace grid
{
    class Grid;
    class AdditionalButtons;
}

namespace midi
{
    class UsbMidi;
}

namespace application
{

class GridTest : private Application, private freertos::Thread
{
public:
    GridTest( grid::Grid& grid, grid::AdditionalButtons& additionalButtons, midi::UsbMidi& usbMidi );

private:
    void initialize();
    void deinitialize();

    void displayBootAnimation();

    Color getBootAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY );
    Color getRandomColor();

    void handleAdditionalButtonEvent( const grid::AdditionalButtons::Event event );
    void handleGridButtonEvent( const grid::Grid::ButtonEvent event );
    void handleMidiPacketAvailable();

    void Run();

    grid::Grid& grid_;
    grid::AdditionalButtons& additionalButtons_;
    midi::UsbMidi& usbMidi_;
};

} // namespace

#endif // APPLICATION_GRID_TEST_H_