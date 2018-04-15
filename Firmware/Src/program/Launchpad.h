/*
 * launchpad.h
 *
 *  Created on: 2018-02-21
 *      Author: Gedas
 */
#ifndef PROGRAM_LAUNCHPAD_H_
#define PROGRAM_LAUNCHPAD_H_

#include "Types.h"
#include "usb/UsbMidi.h"

namespace grid
{
    class Grid;
}

namespace switches
{
    class Switches;
}

namespace gui
{
    class Gui;
}

namespace midi
{
    class UsbMidi;
}

namespace launchpad
{

enum Layout
{
    Layout_SESSION = 0,
    Layout_USER1,
    Layout_USER2,
    Layout_RESERVED,
    Layout_VOLUME,
    Layout_PAN
};

static const uint8_t SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH = 64;

static const uint8_t challengeResponse[10] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7};
static const uint8_t launchpad_standartSystemExclusiveMessageHeader[6] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x18};

static const uint8_t sessionLayout[10][8] = {
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t drumLayout[10][8] = {
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const struct Colour launchpadColourPalette[128] = {
        {0, 0, 0}, {8, 8, 8}, {32, 32, 32}, {64, 64, 64}, {64, 20, 18}, {64, 3, 0}, {23, 1, 0}, {7, 0, 0},
        {64, 48, 25}, {64, 22, 0}, {23, 8, 0}, {10, 7, 0}, {64, 64, 9}, {64, 64, 0}, {23, 23, 0}, {6, 6, 0},
        {33, 64, 11}, {17, 64, 0}, {6, 23, 0}, {5, 11, 0}, {14, 64, 11}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0},
        {13, 64, 18}, {0, 64, 0}, {0, 23, 0}, {0, 7, 0}, {13, 64, 32}, {0, 64, 15}, {0, 23, 6}, {0, 8, 4},
        {12, 64, 45}, {0, 64, 37}, {0, 23, 13}, {0, 7, 4}, {15, 48, 64}, {0, 42, 64}, {0, 17, 21}, {0, 5, 7},
        {17, 34, 64}, {0, 21, 64}, {0, 7, 23}, {0, 2, 7}, {18, 18, 64}, {0, 0, 64}, {0, 0, 23}, {0, 0, 7},
        {33, 18, 64}, {21, 0, 64}, {6, 0, 26}, {3, 0, 13}, {64, 19, 64}, {64, 0, 64}, {23, 0, 23}, {7, 0, 7},
        {64, 20, 34}, {64, 2, 21}, {23, 1, 7}, {9, 0, 5}, {64, 7, 0}, {39, 14, 0}, {31, 21, 0}, {16, 26, 0},
        {0, 15, 0}, {0, 22, 13}, {0, 21, 32}, {0, 0, 64}, {0, 18, 20}, {7, 0, 53}, {32, 32, 32}, {9, 9, 9},
        {64, 3, 0}, {47, 64, 0}, {43, 60, 0}, {22, 64, 0}, {0, 35, 0}, {0, 64, 31}, {0, 42, 64}, {0, 7, 64},
        {14, 0, 64}, {30, 0, 64}, {46, 6, 32}, {17, 9, 0}, {64, 19, 0}, {33, 57, 0}, {26, 64, 0}, {0, 64, 0},
        {0, 64, 0}, {18, 64, 25}, {0, 64, 51}, {21, 34, 64}, {10, 20, 51}, {33, 31, 60}, {53, 4, 64}, {64, 2, 23},
        {64, 32, 0}, {47, 45, 0}, {35, 64, 0}, {33, 24, 0}, {14, 11, 0}, {4, 20, 2}, {0, 21, 14}, {5, 5, 11},
        {5, 8, 23}, {27, 16, 6}, {44, 2, 0}, {64, 21, 14}, {56, 27, 0}, {64, 64, 0}, {39, 57, 0}, {24, 46, 0},
        {7, 7, 13}, {56, 64, 21}, {30, 64, 47}, {38, 38, 64}, {35, 25, 64}, {17, 17, 17}, {30, 30, 30}, {56, 64, 64},
        {42, 2, 0}, {14, 0, 0}, {0, 53, 0}, {0, 17, 0}, {47, 45, 0}, {16, 13, 0}, {46, 24, 0}, {19, 6, 0} };

static const uint8_t topRowControllerNumbers[8] = {4, 7, 5, 6, 3, 2, 0, 1};

enum Launchpad95Mode
{
    Launchpad95Mode_SESSION = 0,
    Launchpad95Mode_INSTRUMENT,
    Launchpad95Mode_DEVICE_CONTROLLER,
    Launchpad95Mode_USER1,
    Launchpad95Mode_DRUM_STEP_SEQUENCER,
    Launchpad95Mode_MELODIC_SEQUENCER,
    Launchpad95Mode_USER2,
    Launchpad95Mode_MIXER,
    Launchpad95Mode_UNKNOWN
};

enum Launchpad95Submode
{
    Launchpad95Submode_DEFAULT = 0,
    Launchpad95Submode_SCALE, // Instrument and Drum step sequencer
    Launchpad95Submode_VOLUME, // Mixer
    Launchpad95Submode_PAN, // Mixer
    Launchpad95Submode_SEND_A, // Mixer
    Launchpad95Submode_SEND_B, // Mixer
    Launchpad95Submode_LENGTH, // Melodic step sequencer
    Launchpad95Submode_OCTAVE, // Melodic step sequencer
    Launchpad95Submode_VELOCITY // Melodic step sequencer
};

class Launchpad
{
public:
    Launchpad( grid::Grid& grid_, switches::Switches& switches_, gui::Gui& gui_, midi::UsbMidi& usbMidi_ );

    void runProgram();
    Launchpad95Mode getLaunchpad95Mode();
    Launchpad95Submode getLaunchpad95Submode();
private:
    void setCurrentLayout(uint8_t layout);
    void processNoteOnMidiMessage( uint8_t channel, uint8_t note, uint8_t velocity );
    void processChangeControlMidiMessage( uint8_t channel, uint8_t control, uint8_t value );
    void processSystemExclusiveMidiPacket( const midi::MidiPacket* packet );
    void processSystemExclusiveMessage(uint8_t *message, uint8_t length);
    void processDawInfoMessage( char* message, uint8_t length );

    void printMidiMessage(midi::MidiPacket* packet);
    void printSysExMessage(uint8_t *message, uint8_t length);

    grid::Grid& grid;
    switches::Switches& switches;
    gui::Gui& gui;
    midi::UsbMidi& usbMidi;


    uint8_t currentLayout = Layout_SESSION;

    Launchpad95Mode currentLaunchpad95Mode = Launchpad95Mode_UNKNOWN; // used only to identify submode

    uint8_t systemExclusiveInputMessage[SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH + 3];
    uint8_t systemExclusiveInputMessageLength = 0;

    int16_t rotaryControlValue[2] = {64, 64};

    //midi::MidiInput midiInput;
};

} // namespace

#endif /* PROGRAM_LAUNCHPAD_H_ */
