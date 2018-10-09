#ifndef USB_USB_MIDI_H_
#define USB_USB_MIDI_H_

#include <stdint.h>

namespace midi
{

struct MidiPacket
{
    uint8_t header;
    uint8_t data[3];
};

union MidiInput
{
    uint32_t input;
    MidiPacket packet;
};

class UsbMidi
{
public:
    UsbMidi();
    ~UsbMidi();

    bool getPacket( MidiPacket& packet);
    bool isPacketAvailable();

    void sendControlChange( const uint8_t channel, const uint8_t control, const uint8_t value );
    void sendNoteOn( const uint8_t channel, const uint8_t note, const uint8_t velocity );
    void sendNoteOff( const uint8_t channel, const uint8_t note );
    void sendSystemExclussive( const uint8_t* const data, const uint8_t length );
};

} // namespace

#endif // USB_USB_MIDI_H_
