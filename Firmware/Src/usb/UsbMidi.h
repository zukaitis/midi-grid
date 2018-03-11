/**
  ******************************************************************************
  * @file           : usbd_midi_if.h
  * @brief          : Header for usbd_midi_if file.
  ******************************************************************************
*/

#ifndef __USBD_MIDI_IF_H
#define __USBD_MIDI_IF_H

extern "C" {
#include "usbd_midi.h"
#include "usb/usbd_desc.h"

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

    void sendNoteOn( const uint8_t channel, const uint8_t note, const uint8_t velocity );
    void sendNoteOff( const uint8_t channel, const uint8_t note );
    void sendControlChange( const uint8_t channel, const uint8_t control, const uint8_t value );
    void sendSystemExclussive( const uint8_t* const data, const uint8_t length );

    bool getPacket( MidiPacket& packet);
    bool isPacketAvailable();
};

} // namespace

#endif /* __USBD_MIDI_IF_H */
