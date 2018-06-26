#include "usb/UsbMidi.h"

#include "stm32f4xx_hal.h"
#include "queue32.h"

// basic midi rx/tx functions
static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
static uint16_t MIDI_DataTx(uint8_t *message, uint16_t length);

// from mi:muz (Internal)
stB4Arrq rxq;

USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS =
{
    MIDI_DataRx,
    MIDI_DataTx
};

static uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length)
{
    const uint16_t msgs = length / 4;
    const uint16_t chk = length % 4;
    if (0 == chk)
    {
        for(uint16_t cnt = 0;cnt < msgs;cnt ++)
        {
            b4arrq_push(&rxq,((uint32_t *)msg)+cnt);
        }
    }
    return 0;
}

static uint16_t MIDI_DataTx(uint8_t *message, uint16_t length)
{
    uint32_t i = 0;
    while (i < length)
    {
        APP_Rx_Buffer[APP_Rx_ptr_in] = *(message + i);
        APP_Rx_ptr_in++;
        i++;
        if (APP_RX_DATA_SIZE == APP_Rx_ptr_in)
        {
            APP_Rx_ptr_in = 0;
        }
    }
    return USBD_OK;
}

namespace midi
{

UsbMidi::UsbMidi()
{
}

UsbMidi::~UsbMidi()
{
}

bool UsbMidi::getPacket( MidiPacket& packet)
{
    MidiInput midiInput;
    bool packetAvailable = false;
    if (0 != rxq.num)
    {
        midiInput.input = *b4arrq_pop(&rxq);
        packet = midiInput.packet;
        packetAvailable = true;
    }
    return packetAvailable;
}

bool UsbMidi::isPacketAvailable()
{
    return (0 != rxq.num);
}

void UsbMidi::sendControlChange( const uint8_t channel, const uint8_t control, const uint8_t value )
{
    static uint8_t buffer[4];
    buffer[0] = 0x0B;
    buffer[1] = 0xB0 | channel;
    buffer[2] = 0x7F & control;
    buffer[3] = 0x7F & value;

    MIDI_DataTx( buffer, 4 );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOn( const uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    static uint8_t buffer[4];
    buffer[0] = 0x09;
    buffer[1] = 0x90 | channel;
    buffer[2] = 0x7F & note;
    buffer[3] = 0x7F & velocity;

    MIDI_DataTx( buffer, 4 );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOff( const uint8_t channel, const uint8_t note )
{
    static uint8_t buffer[4];
    buffer[0] = 0x08;
    buffer[1] = 0x80 | channel;
    buffer[2] = 0x7F & note;
    buffer[3] = 0;

    MIDI_DataTx( buffer, 4 );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendSystemExclussive( const uint8_t* const data, const uint8_t length )
{
    static uint8_t buffer[4];
    uint8_t dataIndex = 0;
    while (dataIndex < length)
    {
        const uint8_t bytesRemaining = length - dataIndex;
        switch (bytesRemaining)
        {
            case 3:
                buffer[0] = 0x07;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = data[dataIndex++];
                break;
            case 2:
                buffer[0] = 0x06;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = 0;
                break;
            case 1:
                buffer[0] = 0x05;
                buffer[1] = data[dataIndex++];
                buffer[2] = 0;
                buffer[3] = 0;
                break;
            default: // more than 3 bytes left
                buffer[0] = 0x04;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = data[dataIndex++];
                break;
        }
        MIDI_DataTx( buffer, 4 );
        USBD_MIDI_SendPacket();
    }
}

} // namespace
