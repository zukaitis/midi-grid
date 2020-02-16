#ifndef APPLICATION_GRID_TEST_HPP_
#define APPLICATION_GRID_TEST_HPP_

#include "application/Application.hpp"
#include "types/Color.h"
#include <freertos/thread.hpp>

namespace grid
{
    class GridInterface;
    class AdditionalButtonsInterface;
}

namespace midi
{
    class UsbMidi;
}

namespace lcd
{
    class LcdInterface;
}

namespace application
{

class GridTest : public Application
{
public:
    GridTest( ApplicationController& applicationController, grid::GridInterface& grid, lcd::LcdInterface& lcd, midi::UsbMidi& usbMidi );

private:
    void run( ApplicationThread& thread ) override;

    void handleAdditionalButtonEvent( const additional_buttons::Event& event ) override;
    void handleGridButtonEvent( const grid::ButtonEvent& event ) override;
    void handleMidiPacketAvailable() override;

    void displayIntroAnimation( ApplicationThread& thread );
    void displayWaitingForMidi();

    Color getIntroAnimationColor( const uint8_t ledPositionX, const uint8_t ledPositionY ) const;
    Color getRandomColor();

    grid::GridInterface& grid_;
    lcd::LcdInterface& lcd_;
};

} // namespace

#endif // APPLICATION_GRID_TEST_HPP_
