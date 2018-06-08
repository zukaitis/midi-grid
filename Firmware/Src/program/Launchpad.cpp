#include "program/launchpad.h"

#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"

#include "usb/usb_device.h"
#include "usb/queue32.h"

namespace launchpad
{

Launchpad::Launchpad( grid::Grid& grid_, switches::Switches& switches_, gui::Gui& gui_, midi::UsbMidi& usbMidi_ ) :
        grid( grid_ ),
        gui( gui_ ),
        switches( switches_ ),
        usbMidi( usbMidi_ )
{};

void Launchpad::runProgram()
{
    uint8_t buttonX, buttonY;
    int8_t rotaryStep;
    ButtonEvent event;
    midi::MidiPacket inputPacket;

    grid.discardAllPendingButtonEvents();
    grid.turnAllLedsOff();
    switches.discardAllPendingEvents();

    usbMidi.sendControlChange( 0, 111, 127 ); // always send control to put device into mixer mode, when entering launchpad mode
    gui.registerMidiOutputActivity();

    gui.enterLaunchpad95Mode();

    while (1)
    {
        if (usbMidi.getPacket( inputPacket ))
        {
            const uint8_t codeIndexNumber = inputPacket.header & 0x0F;
            switch (codeIndexNumber)
            {
                case 0x09: // note on
                    processNoteOnMidiMessage( inputPacket.data[0] & 0x0F, inputPacket.data[1], inputPacket.data[2] );
                    break;
                case 0x08: // note off
                    processNoteOnMidiMessage( inputPacket.data[0] & 0x0F, inputPacket.data[1], 0 );
                    break;
                case 0x0B: // change control
                    processChangeControlMidiMessage( inputPacket.data[0] & 0x0F, inputPacket.data[1], inputPacket.data[2] );
                    break;
                case 0x04: // system exclusive
                case 0x05:
                case 0x06:
                case 0x07:
                    processSystemExclusiveMidiPacket( inputPacket );
                    break;
                default:
                    printMidiMessage( inputPacket );
                    break;
            }
            gui.registerMidiInputActivity();
        }

        if (grid.getButtonEvent( buttonX, buttonY, event ))
        {
            const uint8_t velocity = (ButtonEvent_PRESSED == event) ? 127 : 0;
            if (9 == buttonX) // control row
            {
                usbMidi.sendControlChange( 0, sessionLayout[buttonX][buttonY], velocity );
            }
            else
            {
                switch (currentLayout_)
                {
                    case Layout_SESSION:
                        usbMidi.sendNoteOn( 0, sessionLayout[buttonX][buttonY], velocity );
                        break;
                    case Layout_USER1:
                        usbMidi.sendNoteOn( 7, drumLayout[buttonX][buttonY], velocity ); // can select channel between 6, 7 and 8
                        break;
                    case Layout_USER2:
                        usbMidi.sendNoteOn( 15, sessionLayout[buttonX][buttonY], velocity ); // can select channel between 14, 15 and 16
                        break;
                    default:
                        usbMidi.sendNoteOn( 0, sessionLayout[buttonX][buttonY], velocity );
                        break;
                }
            }
            gui.registerMidiOutputActivity();
        }

        if (switches.getButtonEvent( buttonX,  event ))
        {
            if (0 == buttonX) // only send note on the event of black button
            {
                const uint8_t velocity = (ButtonEvent_PRESSED == event) ? 127 : 0;
                usbMidi.sendNoteOn( 5, 55, velocity );
                gui.registerMidiOutputActivity();
            }
            else if ((1 == buttonX) && event)
            {
                break; // break launchpad mode loop, enter internal menu
            }
        }

        if (switches.getRotaryEncoderEvent( buttonX, rotaryStep ))
        {
            rotaryControlValue_[buttonX] += rotaryStep;
            if (rotaryControlValue_[buttonX] > 127)
            {
                rotaryControlValue_[buttonX] = 127;
            }
            else if (rotaryControlValue_[buttonX] < 0)
            {
                rotaryControlValue_[buttonX] = 0;
            }
            usbMidi.sendControlChange( 5, buttonX, rotaryControlValue_[buttonX] ); // randomly selected channel and control values
            gui.registerMidiOutputActivity();
            gui.displayRotaryControlValues( rotaryControlValue_[0], rotaryControlValue_[1] );
        }

        grid.refreshLeds();
        gui.refresh();
    }
}

Launchpad95Mode Launchpad::getLaunchpad95Mode()
{
    Launchpad95Mode mode = Launchpad95Mode_UNKNOWN;
    Colour colour;

    do
    {
        colour = grid.getLedColour( 9, 3 ); // session led
        if (grid.areColoursEqual( colour, launchpadColourPalette[21] ))
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        colour = grid.getLedColour( 9, 2 ); // user1 led
        if (grid.areColoursEqual( colour, launchpadColourPalette[37] ))
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        else if (grid.areColoursEqual( colour, launchpadColourPalette[48] ))
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        else if (grid.areColoursEqual( colour, launchpadColourPalette[45] ))
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        colour = grid.getLedColour( 9, 0 );
        if (grid.areColoursEqual( colour, launchpadColourPalette[53] ))
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        else if (grid.areColoursEqual( colour, launchpadColourPalette[9] ))
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        else if (grid.areColoursEqual( colour, launchpadColourPalette[45] ))
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        colour = grid.getLedColour( 9, 1 ); // mixer led
        if (grid.areColoursEqual( colour, launchpadColourPalette[29] ))
        {
            mode = Launchpad95Mode_MIXER;
            break;
        }
    } while (false);

    return mode;
}

Launchpad95Submode Launchpad::getLaunchpad95Submode()
{
    Launchpad95Submode submode = Launchpad95Submode_DEFAULT;
    Colour colour;

    switch (currentLaunchpad95Mode_)
    {
        case Launchpad95Mode_INSTRUMENT:
            colour = grid.getLedColour( 8, 7 );
            if (grid.areColoursEqual( colour, launchpadColourPalette[9] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            colour = grid.getLedColour( 8, 7 );
            if (grid.areColoursEqual( colour, launchpadColourPalette[5] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_MIXER:
            do
            {
                colour = grid.getLedColour( 8, 7 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[31] ))
                {
                    submode = Launchpad95Submode_VOLUME;
                    break;
                }
                colour = grid.getLedColour( 8, 6 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[31] ))
                {
                    submode = Launchpad95Submode_PAN;
                    break;
                }
                colour = grid.getLedColour( 8, 5 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_A;
                    break;
                }
                colour = grid.getLedColour( 8, 4 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_B;
                }
            } while (false);
            break;
        case Launchpad95Mode_MELODIC_SEQUENCER:
            do
            {
                colour = grid.getLedColour( 8, 3 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[29] ))
                {
                    submode = Launchpad95Submode_LENGTH;
                    break;
                }
                colour = grid.getLedColour( 8, 2 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[48] ))
                {
                    submode = Launchpad95Submode_OCTAVE;
                    break;
                }
                colour = grid.getLedColour( 8, 1 );
                if (grid.areColoursEqual( colour, launchpadColourPalette[37] ))
                {
                    submode = Launchpad95Submode_VELOCITY;
                }
            } while (false);
            break;
        default:
            break;
    }

    return submode;
}

void Launchpad::processDawInfoMessage( const char* const message, uint8_t length )
{
    switch (message[0])
    {
        case 't':
            gui.setDawTrackName( &message[1], length - 1 );
            break;
        case 'c':
            gui.setDawClipName( &message[1], length - 1 );
            break;
        case 'd':
            gui.setDawDeviceName( &message[1], length - 1 );
            break;
        case 's':
            gui.setDawStatus( ('P' == message[1]), ('R' == message[2]), ('S' == message[3]) );
            break;
        case 'T':
            {
                const uint16_t tempo = (message[1] - '0')*100 + (message[2] - '0')*10 + (message[3] - '0');
                const uint8_t signatureNumerator = (message[4] - '0')*10 + (message[5] - '0');
                const uint8_t signatureDenominator = (message[6] - '0')*10 + (message[7] - '0');
                gui.setDawTimingValues( tempo, signatureNumerator, signatureDenominator,
                        ('D' == message[8]), ('U' == message[8]) );
            }
            break;
        default:
            break;
    }
}

void Launchpad::processChangeControlMidiMessage( uint8_t channel, uint8_t control, uint8_t value )
{
    if ((control >= 104) && (control <= 111))
    {
        const uint8_t ledPositionX = 9;
        const uint8_t ledPositionY = topRowControllerNumbers[control - 104];
        grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[value], static_cast<grid::LedLightingType>(channel) );
        if (ledPositionY <= 3)
        {
            currentLaunchpad95Mode_ = getLaunchpad95Mode();
            gui.setLaunchpad95Mode( currentLaunchpad95Mode_ );
            if (Launchpad95Mode_MELODIC_SEQUENCER == currentLaunchpad95Mode_)
            {
                // only melodic step sequencer can stay in submode between mode changes
                gui.setLaunchpad95Submode( getLaunchpad95Submode() );
            }
            else
            {
                gui.setLaunchpad95Submode( Launchpad95Submode_DEFAULT );
            }
        }
    }
}

void Launchpad::processNoteOnMidiMessage( uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    uint8_t ledPositionX, ledPositionY;
    if (Layout_USER1 == currentLayout_)
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

            if (velocity < LAUNCHPAD_COLOUR_PALETTE_SIZE)
            {
                grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[velocity], static_cast<grid::LedLightingType>(channel) );
            }
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

            if (velocity < LAUNCHPAD_COLOUR_PALETTE_SIZE)
            {
                grid.setLed( ledPositionX, ledPositionY, launchpadColourPalette[velocity], static_cast<grid::LedLightingType>(channel) );
            }

            if (8 == ledPositionX)
            {
                // possible change in submode
                gui.setLaunchpad95Submode( getLaunchpad95Submode() );
            }
#if 0 // debug
            char str[4];
            sprintf(str, "%03i", velocity);
            if (89 == note)
            {
                lcd::Lcd::getInstance().print(str,10, 20);
            }
#endif
        }
    }
}

void Launchpad::processSystemExclusiveMessage( uint8_t* const message, uint8_t length )
{
    if (length > 7)
    {
        if (0 == memcmp( message, launchpad_standartSystemExclusiveMessageHeader, 6 ))
        {
            switch(message[6])
            {
                case 0x22:
                    setCurrentLayout( message[7] );
                    break;
                case 0x40:
                    usbMidi.sendSystemExclussive( &challengeResponse[0], 10 ); // always return zeros as challenge response
                    gui.registerMidiOutputActivity();
                    break;
                case 0x14: // text scroll
                    message[length-1] = 0; // put string terminator at the end
                    processDawInfoMessage( reinterpret_cast<char*>(&message[7]), length - 7 - 1 );
                    break;
                default:
                    printSysExMessage( message, length );
                    break;
            }
        }
        else
        {
            printSysExMessage( message, length );
        }
    }
    else
    {
        printSysExMessage( message, length );
    }
}

void Launchpad::processSystemExclusiveMidiPacket( const midi::MidiPacket& packet )
{
    const uint8_t codeIndexNumber = packet.header & 0x0F;

    if (systemExclusiveInputMessageLength_ >= (SYSTEM_EXCLUSIVE_MESSAGE_MAXIMUM_LENGTH - 3))
    {
        systemExclusiveInputMessageLength_ = 0; // discard this message, as it is too long
    }
    else
    {
        switch (codeIndexNumber)
        {
            case 0x04:
            case 0x07:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[1];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[2];
                break;
            case 0x05:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                break;
            case 0x06:
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[0];
                systemExclusiveInputMessage_[systemExclusiveInputMessageLength_++] = packet.data[1];
                break;
            default:
                break;
        }

        if ((0x05 == codeIndexNumber) || (0x06 == codeIndexNumber) || (0x07 == codeIndexNumber))
        {
            processSystemExclusiveMessage( &systemExclusiveInputMessage_[0], systemExclusiveInputMessageLength_ );
            systemExclusiveInputMessageLength_ = 0; // reset message length
        }
    }
}

void Launchpad::setCurrentLayout( uint8_t layout )
{
    if (layout <= Layout_PAN)
    {
        currentLayout_ = static_cast<Layout>(layout);
    }
}

void Launchpad::printMidiMessage( midi::MidiPacket& packet )
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

void Launchpad::printSysExMessage( const uint8_t* const message, uint8_t length )
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
