/*
 * launchpad.c
 *
 *  Created on: 2018-02-21
 *      Author: Gedas
 */

#include "program/launchpad.h"

#include "grid/Grid.h"
#include "lcd/Gui.h"

extern "C" {
#include "usb/usb_device.h"
#include "usb/queue32.h"
};



//extern stB4Arrq rxq;

namespace launchpad
{

Launchpad::Launchpad( grid::Grid& grid_, gui::Gui& gui_, midi::UsbMidi& usbMidi_ ) :
        grid( grid_ ),
        gui( gui_ ),
        usbMidi( usbMidi_ )
{};

void Launchpad::runProgram()
{
    uint8_t buttonX, buttonY, velocity;
    grid::ButtonEvent event;
    uint8_t codeIndexNumber;
    midi::MidiPacket inputPacket;
    while (1)
    {
        // led flash message - 0x15519109 (Hex)

        if (usbMidi.getPacket(inputPacket))
        {
            //midiInput.input = *b4arrq_pop(&rxq);
            codeIndexNumber = inputPacket.header & 0x0F;
            switch (codeIndexNumber)
            {
                case 0x09: // note on
                    processNoteOnMidiMessage(inputPacket.data[0] & 0x0F, inputPacket.data[1], inputPacket.data[2]);
                    break;
                case 0x08: // note off
                    processNoteOnMidiMessage(inputPacket.data[0] & 0x0F, inputPacket.data[1], 0);
                    break;
                case 0x0B: // change control
                    processChangeControlMidiMessage(inputPacket.data[0] & 0x0F, inputPacket.data[1], inputPacket.data[2]);
                    break;
                case 0x04: // system exclusive
                case 0x05:
                case 0x06:
                case 0x07:
                    processSystemExclusiveMidiPacket( &inputPacket );
                    break;
                default:
                    printMidiMessage(&inputPacket);
                    break;
            }
            gui.registerMidiInputActivity();
        }

        if (grid.getButtonEvent(&buttonX, &buttonY, &event))
        {
            velocity = (grid::ButtonEvent_PRESSED == event) ? 127 : 0;
            if (9 == buttonX) // control row
            {
                usbMidi.sendControlChange( 0,sessionLayout[buttonX][buttonY],velocity );
            }
            else
            {
                switch (currentLayout)
                {
                    case Layout_SESSION:
                        usbMidi.sendNoteOn( 0, sessionLayout[buttonX][buttonY], velocity );
                        break;
                    case Layout_USER1:
                        usbMidi.sendNoteOn( 7, drumLayout[buttonX][buttonY],velocity ); // can select channel between 6, 7 and 8
                        break;
                    case Layout_USER2:
                        usbMidi.sendNoteOn( 15, sessionLayout[buttonX][buttonY],velocity ); // can select channel between 14, 15 and 16
                        break;
                    default:
                        usbMidi.sendNoteOn( 0, sessionLayout[buttonX][buttonY], velocity );
                        break;
                }
            }
            gui.registerMidiOutputActivity();
        }
        grid.refreshLeds();
        gui.refresh();
    }
}

void Launchpad::processNoteOnMidiMessage(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t ledPositionX, ledPositionY;
    if (Layout_USER1 == currentLayout)
    {
        // only this layout uses drum layout
        if ((note >= 36) && (note <= 107))
        {
            if (note <= 67)
            {
                ledPositionX = note % 4;
                ledPositionY = (note - 36) / 4;
            }
            else if (note <= 99)
            {
                ledPositionX = note % 4 + 4;
                ledPositionY = (note - 68) / 4;
            }
            else
            {
                ledPositionX = 8;
                ledPositionY = 107 - note;
            }

            if (channel > 2)
            {
                channel = 0;
            }

            grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[velocity], static_cast<grid::LedLightingType>(channel) );
        }
    }
    else
    {
        // not sure if this conditional is needed
        if ((note >= 11) && (note <= 89))
        {
            ledPositionX = (note % 10) - 1;
            ledPositionY = (note / 10) - 1;

            if (channel > 2)
            {
                channel = 0;
            }

            grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[velocity], static_cast<grid::LedLightingType>(channel) );
        }
    }
}

void Launchpad::processChangeControlMidiMessage(uint8_t channel, uint8_t control, uint8_t value)
{
    uint8_t ledPositionX, ledPositionY;
    if ((control >= 104) && (control <= 111))
    {
        ledPositionX = 9;
        ledPositionY = topRowControllerNumbers[control - 104];
        grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[value], static_cast<grid::LedLightingType>(channel) );
        gui.displayLaunchpad95Mode( getLaunchpad95Mode() );
    }
}

void Launchpad::processSystemExclusiveMidiPacket( const midi::MidiPacket* packet )
{
    uint8_t codeIndexNumber = packet->header & 0x0F;
    if (0x4 == codeIndexNumber) // start or continuation of SysEx message
    {
        systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[0];
        systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[1];
        systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[2];
        if (systemExclusiveInputMessageLength >= SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH)
        {
            systemExclusiveInputMessageLength = 0; // discard this message, as it is too long
        }
    }
    else // end of SysEx
    {
        if (0x5 == codeIndexNumber)
        {
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[0];
        }
        else if (0x6 == codeIndexNumber)
        {
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[0];
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[1];
        }
        else
        {
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[0];
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[1];
            systemExclusiveInputMessage[systemExclusiveInputMessageLength++] = packet->data[2];
        }
        processSystemExclusiveMessage(&systemExclusiveInputMessage[0], systemExclusiveInputMessageLength);
        systemExclusiveInputMessageLength = 0; // reset message length
    }
}

void Launchpad::processSystemExclusiveMessage( uint8_t *message, uint8_t length )
{
    if (length > 7)
    {
        if (0 == memcmp(message, launchpad_standartSystemExclusiveMessageHeader, 6))
        {
            if (0x22 == message[6])
            {
                setCurrentLayout( message[7] );
            }
            else if (0x40 == message[6])
            {
                usbMidi.sendSystemExclussive( &challengeResponse[0], 10 ); // always return zeros as challenge response
                gui.registerMidiOutputActivity();
            }
            else
            {
                printSysExMessage(message, length);
            }
        }
        else
        {
            printSysExMessage(message, length);
        }
    }
    else
    {
        printSysExMessage(message, length);
    }
}

void Launchpad::setCurrentLayout( uint8_t layout )
{
    if (layout < 6)
    {
        currentLayout = layout;
        // unnecessary stuff below
        Colour colour = {0, 0, 0};
        switch (currentLayout)
        {
            case Layout_SESSION:
                colour.Red = 64;
                break;
            case Layout_USER1:
                colour.Green = 64;
                break;
            case Layout_USER2:
                colour.Blue = 64;
                break;
            case Layout_RESERVED:
                colour.Red = 64;
                colour.Green = 64;
                break;
            case Layout_PAN:
                colour.Red = 64;
                colour.Blue = 64;
                break;
            case Layout_VOLUME:
                colour.Green = 64;
                colour.Blue = 64;
                break;
        }
        grid.setLed( 9, 4, colour );
    }
}

Launchpad95Mode Launchpad::getLaunchpad95Mode()
{
    enum Launchpad95Mode mode = Launchpad95Mode_UNKNOWN;
    Colour colour;

    do
    {
        colour = grid.getLedColour(9, 3); // session led
        if (grid.areColoursEqual(colour, launchpadColourPalette[21]))
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        colour = grid.getLedColour(9, 2); // user1 led
        if (grid.areColoursEqual(colour, launchpadColourPalette[37]))
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        else if (grid.areColoursEqual(colour, launchpadColourPalette[48]))
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        else if (grid.areColoursEqual(colour, launchpadColourPalette[45]))
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        colour = grid.getLedColour(9, 0); // user2 led
        if (grid.areColoursEqual(colour, launchpadColourPalette[53]))
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        else if (grid.areColoursEqual(colour, launchpadColourPalette[9]))
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        else if (grid.areColoursEqual(colour, launchpadColourPalette[45]))
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        colour = grid.getLedColour(9, 1); // mixer led
        if (grid.areColoursEqual(colour, launchpadColourPalette[29]))
        {
            mode = Launchpad95Mode_MIXER;
            break;
        }
    } while (0);

    if (Launchpad95Mode_UNKNOWN != mode)
    {
        currentLaunchpad95Mode = mode;
    }

    return mode;
}

void Launchpad::printMidiMessage(midi::MidiPacket* packet)
{
#ifdef USE_SEMIHOSTING
    uint8_t channel;
    uint8_t codeIndexNumber = midiInput.packet.header & 0x0F;
    if (0x09 == codeIndexNumber)
    {
        channel = midiInput.packet.data[0] & 0x0F;
        printf("NO, ch:%i n:%i v:%i\n", channel, midiInput.packet.data[1], midiInput.packet.data[2]);
    }
    else if (0x0B == codeIndexNumber)
    {
        channel = midiInput.packet.data[0] & 0x0F;
        printf("CC, ch:%i c:%i v:%i\n", channel, midiInput.packet.data[1], midiInput.packet.data[2]);
    }
    else if (0x04 == codeIndexNumber)
    {
        if ((0x2000F004 == midiInput.input) || (0x18022904 == midiInput.input))
        {
            // ignore SysEx header messages
        }
        else
        {
            printf("SE, d: %02Xh %02Xh %02Xh\n", midiInput.packet.data[0], midiInput.packet.data[1], midiInput.packet.data[2]);
        }
    }
    else if (0x07 == codeIndexNumber)
    {
        printf("SEe, d: %02Xh %02Xh %02Xh\n", midiInput.packet.data[0], midiInput.packet.data[1], midiInput.packet.data[2]);
    }
    else
    {
        printf("Unknown message, CIN: %Xh\n", codeIndexNumber);
    }
#endif
}

void Launchpad::printSysExMessage(uint8_t *message, uint8_t length)
{
#ifdef USE_SEMIHOSTING
    printf("SysEx:");
    for (uint8_t i=0; i<length; i++)
    {
        printf(" %02Xh", message[i]);
    }
    printf("\n");
#endif
}

} // namespace
