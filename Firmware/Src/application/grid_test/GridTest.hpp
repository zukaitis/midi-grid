#ifndef APPLICATION_GRID_TEST_HPP_
#define APPLICATION_GRID_TEST_HPP_

#include "application/Application.hpp"
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

namespace lcd
{
    class Lcd;
}

namespace application
{

class GridTest : public Application
{
public:
    GridTest( ApplicationController& applicationController, grid::Grid& grid, lcd::Lcd& lcd, midi::UsbMidi& usbMidi );

private:
    void run( ApplicationThread& thread );

    void handleAdditionalButtonEvent( const AdditionalButtons::Event event );
    void handleGridButtonEvent( const grid::Grid::ButtonEvent event );
    void handleMidiPacketAvailable();

    void displayIntroAnimation( ApplicationThread& thread );
    void displayWaitingForMidi();

    Color getIntroAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY );
    Color getRandomColor();

    grid::Grid& grid_;
    lcd::Lcd& lcd_;
};

} // namespace

#endif // APPLICATION_GRID_TEST_HPP_
