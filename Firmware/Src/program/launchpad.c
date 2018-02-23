/*
 * launchpad.c
 *
 *  Created on: 2018-02-21
 *      Author: Gedas
 */
#include "program/launchpad.h"

#include "grid_buttons/grid_buttons.h"
#include "main.h"
#include "lcd/lcd.h"
#include "usb/usb_device.h"
#include "usb/queue32.h"

#define SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH 64

void setCurrentLayout(uint8_t layout);

extern stB4Arrq rxq;

struct MidiPacket
{
    uint8_t header;
    uint8_t data[3];
};

static const uint8_t challengeResponse[10] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7};
static const uint8_t launchpad_standartSystemExclusiveMessageHeader[6] = {0xF0, 0x00, 0x20, 0x29, 0x02, 0x18};

union MidiInput
{
    uint32_t input;
    struct MidiPacket packet;
};

union MidiInput midiInput;

enum Layout
{
    Layout_SESSION = 0,
    Layout_USER1,
    Layout_USER2,
    Layout_RESERVED,
    Layout_VOLUME,
    Layout_PAN
};

static uint8_t currentLayout = Layout_SESSION;

static uint8_t systemExclusiveInputMessage[SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH + 3];
static uint8_t systemExclusiveInputMessageLength = 0;

void processNoteOnMidiMessage( uint8_t channel, uint8_t note, uint8_t velocity );
void processChangeControlMidiMessage( uint8_t channel, uint8_t control, uint8_t value );
void processSystemExclusiveMidiPacket( const struct MidiPacket* packet );
void processSystemExclusiveMessage(uint8_t *message, uint8_t length);

void printMidiMessage(union MidiInput message);
void printSysExMessage(uint8_t *message, uint8_t length);

void launchpad_runProgram()
{
    uint8_t buttonX, buttonY, event, velocity;
    uint8_t ledPositionX, ledPositionY;
    uint8_t codeIndexNumber, channel;
    while (1)
    {
        // led flash message - 0x15519109 (Hex)

        if (0 != rxq.num)
        {
            midiInput.input = *b4arrq_pop(&rxq);
            codeIndexNumber = midiInput.packet.header & 0x0F;
            if (0x9 == codeIndexNumber) // note on
            {
                processNoteOnMidiMessage(midiInput.packet.data[0] & 0x0F, midiInput.packet.data[1], midiInput.packet.data[2]);
            }
            else if (0x8 == codeIndexNumber) // note off
            {
                processNoteOnMidiMessage(midiInput.packet.data[0] & 0x0F, midiInput.packet.data[1], 0);
            }
            else if (0xB == codeIndexNumber) // change control
            {
                processChangeControlMidiMessage(midiInput.packet.data[0] & 0x0F, midiInput.packet.data[1], midiInput.packet.data[2]);
            }
            else if ((codeIndexNumber >= 0x4) && (codeIndexNumber <= 0x7))
            {
                processSystemExclusiveMidiPacket( &midiInput.packet );
            }
            else
            {
                printMidiMessage(midiInput);
            }
        }

        if (grid_getButtonEvent(&buttonX, &buttonY, &event))
        {
            velocity = (event != 0) ? 127 : 0;
            if (9 == buttonX) // control row
            {
                sendCtlChange( 0,sessionLayout[buttonX][buttonY],velocity );
            }
            else
            {
                if (Layout_SESSION == currentLayout)
                {
                    sendNoteOn( 0,sessionLayout[buttonX][buttonY],velocity );
                }
                else if (Layout_USER1 == currentLayout)
                {
                    sendNoteOn( 7, drumLayout[buttonX][buttonY],velocity ); // can select channel between 6, 7 and 8
                }
                else if (Layout_USER2 == currentLayout)
                {
                    sendNoteOn( 15, sessionLayout[buttonX][buttonY],velocity ); // can select channel between 14, 15 and 16
                }
                else //if ((Layout_VOLUME == currentLayout) || (Layout_PAN == currentLayout))
                {
                    sendNoteOn( 0,sessionLayout[buttonX][buttonY],velocity );
                }
  //                else
  //                {
  //                    //don't send anything
  //                }
            }
              //LCD_print("zdrw jums", 12, 2);
        }
        grid_updateLeds();
    }
}

void processNoteOnMidiMessage(uint8_t channel, uint8_t note, uint8_t velocity)
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

            grid_setLedFromMidiMessage(ledPositionX, ledPositionY, velocity, channel);
        }
        else
        {
            printMidiMessage(midiInput);
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

            grid_setLedFromMidiMessage(ledPositionX, ledPositionY, velocity, channel);
        }
        else
        {
            printMidiMessage(midiInput);
        }
    }
}

void processChangeControlMidiMessage(uint8_t channel, uint8_t control, uint8_t value)
{
    uint8_t ledPositionX, ledPositionY;
    if ((control >= 104) && (control <= 111))
    {
        ledPositionX = 9;
        ledPositionY = topRowControllerNumbers[control - 104];
        grid_setLedFromMidiMessage(ledPositionX, ledPositionY, value, 0);
    }
    else
    {
        printMidiMessage(midiInput);
    }
}

void processSystemExclusiveMidiPacket( const struct MidiPacket* packet )
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

void processSystemExclusiveMessage( uint8_t *message, uint8_t length )
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
                sendSysEx( &challengeResponse[0], 10 ); // always return zeros as challenge response
            }
        }
    }
    else
    {
        printSysExMessage(message, length);
    }
}

void setCurrentLayout( uint8_t layout )
{
    if (layout < 6)
    {
        currentLayout = layout;
        struct Colour colour = {0, 0, 0};
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
        grid_setLedColour( 9, 4, &colour );
    }
}

void printMidiMessage(union MidiInput message)
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

void printSysExMessage(uint8_t *message, uint8_t length)
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
