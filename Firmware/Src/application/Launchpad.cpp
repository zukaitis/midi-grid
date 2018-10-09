#include "application/Launchpad.h"

#include "grid/Grid.h"
#include "grid/Switches.h"
#include "lcd/Gui.h"
#include "lcd/Lcd.h"

#include "usb/usb_device.h"
#include "usb/queue32.h"

namespace launchpad
{

static const int16_t kMidiControlMinimumValue = 0;
static const int16_t kMidiControlMaximumValue = 127;

static const uint8_t kChallengeResponseLength = 10;
static const uint8_t kChallengeResponse[kChallengeResponseLength] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7 };

static const uint8_t kStandardSystemExclussiveMessageHeaderLength = 6;
static const uint8_t kStandardSystemExclussiveMessageHeader[kStandardSystemExclussiveMessageHeaderLength] =
        { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18 };

static const uint8_t kSessionLayout[10][8] = {
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t kDrumLayout[10][8] = {
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} };

static const uint8_t kLaunchpadColorPaletteSize = 128;

static const Color kLaunchpadColorPalette[kLaunchpadColorPaletteSize] = {
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

static const uint8_t kTopRowControllerNumber[8] = {4, 7, 5, 6, 3, 2, 0, 1};

Launchpad::Launchpad( grid::Grid& grid_, grid::Switches& switches_, lcd::Gui& gui_, midi::UsbMidi& usbMidi_ ) :
        grid( grid_ ),
        switches( switches_ ),
        gui( gui_ ),
        usbMidi( usbMidi_ ),
        currentLaunchpad95Mode_( Launchpad95Mode_UNKNOWN ),
        currentLayout_( Layout_SESSION ),
        systemExclusiveInputMessageLength_( 0 )
{
    rotaryControlValue_[0] = 64;
    rotaryControlValue_[1] = 64;
}

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
                    break;
            }
            gui.registerMidiInputActivity();
        }

        if (grid.getButtonEvent( buttonX, buttonY, event ))
        {
            const uint8_t velocity = (ButtonEvent_PRESSED == event) ? 127 : 0;
            if (9 == buttonX) // control row
            {
                usbMidi.sendControlChange( 0, kSessionLayout[buttonX][buttonY], velocity );
            }
            else
            {
                switch (currentLayout_)
                {
                    case Layout_SESSION:
                        usbMidi.sendNoteOn( 0, kSessionLayout[buttonX][buttonY], velocity );
                        break;
                    case Layout_USER1:
                        usbMidi.sendNoteOn( 7, kDrumLayout[buttonX][buttonY], velocity ); // can select channel between 6, 7 and 8
                        break;
                    case Layout_USER2:
                        usbMidi.sendNoteOn( 15, kSessionLayout[buttonX][buttonY], velocity ); // can select channel between 14, 15 and 16
                        break;
                    default:
                        usbMidi.sendNoteOn( 0, kSessionLayout[buttonX][buttonY], velocity );
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
            if (rotaryControlValue_[buttonX] > kMidiControlMaximumValue)
            {
                rotaryControlValue_[buttonX] = kMidiControlMaximumValue;
            }
            else if (rotaryControlValue_[buttonX] < kMidiControlMinimumValue)
            {
                rotaryControlValue_[buttonX] = kMidiControlMinimumValue;
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
    Color color;

    do
    {
        color = grid.getLedColor( 9, 3 ); // session led
        if (grid.areColorsEqual( color, kLaunchpadColorPalette[21] ))
        {
            mode = Launchpad95Mode_SESSION;
            break;
        }

        color = grid.getLedColor( 9, 2 ); // user1 led
        if (grid.areColorsEqual( color, kLaunchpadColorPalette[37] ))
        {
            mode = Launchpad95Mode_INSTRUMENT;
            break;
        }
        else if (grid.areColorsEqual( color, kLaunchpadColorPalette[48] ))
        {
            mode = Launchpad95Mode_DEVICE_CONTROLLER;
            break;
        }
        else if (grid.areColorsEqual( color, kLaunchpadColorPalette[45] ))
        {
            mode = Launchpad95Mode_USER1;
            break;
        }

        color = grid.getLedColor( 9, 0 );
        if (grid.areColorsEqual( color, kLaunchpadColorPalette[53] ))
        {
            mode = Launchpad95Mode_DRUM_STEP_SEQUENCER;
            break;
        }
        else if (grid.areColorsEqual( color, kLaunchpadColorPalette[9] ))
        {
            mode = Launchpad95Mode_MELODIC_SEQUENCER;
            break;
        }
        else if (grid.areColorsEqual( color, kLaunchpadColorPalette[45] ))
        {
            mode = Launchpad95Mode_USER2;
            break;
        }

        color = grid.getLedColor( 9, 1 ); // mixer led
        if (grid.areColorsEqual( color, kLaunchpadColorPalette[29] ))
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
    Color color;

    switch (currentLaunchpad95Mode_)
    {
        case Launchpad95Mode_INSTRUMENT:
            color = grid.getLedColor( 8, 7 );
            if (grid.areColorsEqual( color, kLaunchpadColorPalette[9] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_DRUM_STEP_SEQUENCER:
            color = grid.getLedColor( 8, 7 );
            if (grid.areColorsEqual( color, kLaunchpadColorPalette[5] ))
            {
                submode = Launchpad95Submode_SCALE;
            }
            break;
        case Launchpad95Mode_MIXER:
            do
            {
                color = grid.getLedColor( 8, 7 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_VOLUME;
                    break;
                }
                color = grid.getLedColor( 8, 6 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_PAN;
                    break;
                }
                color = grid.getLedColor( 8, 5 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_A;
                    break;
                }
                color = grid.getLedColor( 8, 4 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[31] ))
                {
                    submode = Launchpad95Submode_SEND_B;
                }
            } while (false);
            break;
        case Launchpad95Mode_MELODIC_SEQUENCER:
            do
            {
                color = grid.getLedColor( 8, 3 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[29] ))
                {
                    submode = Launchpad95Submode_LENGTH;
                    break;
                }
                color = grid.getLedColor( 8, 2 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[48] ))
                {
                    submode = Launchpad95Submode_OCTAVE;
                    break;
                }
                color = grid.getLedColor( 8, 1 );
                if (grid.areColorsEqual( color, kLaunchpadColorPalette[37] ))
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
        const uint8_t ledPositionY = kTopRowControllerNumber[control - 104];
        grid.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[value], static_cast<grid::LedLightingType>(channel) );
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

            if (velocity < kLaunchpadColorPaletteSize)
            {
                grid.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[velocity], static_cast<grid::LedLightingType>(channel) );
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

            if (velocity < kLaunchpadColorPaletteSize)
            {
                grid.setLed( ledPositionX, ledPositionY, kLaunchpadColorPalette[velocity], static_cast<grid::LedLightingType>(channel) );
            }

            if (8 == ledPositionX)
            {
                // possible change in submode
                gui.setLaunchpad95Submode( getLaunchpad95Submode() );
            }
        }
    }
}

void Launchpad::processSystemExclusiveMessage( uint8_t* const message, uint8_t length )
{
    if (length > 7)
    {
        if (0 == memcmp( message, kStandardSystemExclussiveMessageHeader, kStandardSystemExclussiveMessageHeaderLength ))
        {
            switch(message[6])
            {
                case 0x22:
                    setCurrentLayout( message[7] );
                    break;
                case 0x40:
                    usbMidi.sendSystemExclussive( &kChallengeResponse[0], kChallengeResponseLength ); // always return zeros as challenge response
                    gui.registerMidiOutputActivity();
                    break;
                case 0x14: // text scroll
                    message[length-1] = 0; // put string terminator at the end
                    processDawInfoMessage( reinterpret_cast<char*>(&message[7]), length - 7 - 1 );
                    break;
                default:
                    break;
            }
        }
    }
}

void Launchpad::processSystemExclusiveMidiPacket( const midi::MidiPacket& packet )
{
    const uint8_t codeIndexNumber = packet.header & 0x0F;

    if (systemExclusiveInputMessageLength_ >= (systemExclussiveMessageMaximumLength_ - 3))
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

} // namespace
