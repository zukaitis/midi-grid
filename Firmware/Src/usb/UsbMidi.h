#ifndef USB_USB_MIDI_H_
#define USB_USB_MIDI_H_

#include "usbd_midi.h"
#include "usb/usbd_desc.h"

extern "C" {
extern uint8_t USBD_MIDI_SendData (USBD_HandleTypeDef *pdev, uint8_t *pBuf, uint16_t length);
extern void USBD_MIDI_SendPacket(void);
}

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
