#include "usb/UsbMidi.hpp"

#include "stm32f4xx_hal.h"
#include "queue.hpp"

#include "usbd_midi.h"
#include "usb/usbd_desc.h"

extern "C" {
extern uint8_t USBD_MIDI_SendData (USBD_HandleTypeDef *pdev, uint8_t *pBuf, uint16_t length);
extern void USBD_MIDI_SendPacket(void);
}

USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS =
{
    midi::UsbMidi::receiveData,
    midi::UsbMidi::transmitData
};

namespace midi
{

static const uint16_t kMidiPacketSize = 4;
static const uint8_t kNoteMask = 0x7F;
static const uint8_t kVelocityMask = 0x7F;
static const uint8_t kControlMask = 0x7F;
static const uint8_t kControlValueMask = 0x7F;

freertos::Queue UsbMidi::receivedMessages = freertos::Queue( 256, sizeof(uint32_t) );

UsbMidi::UsbMidi()
{
}

UsbMidi::~UsbMidi()
{
}

bool UsbMidi::waitForPacket( MidiPacket& packet )
{
    const bool packetAvailable = receivedMessages.Dequeue( &packet ); // block until event
    return packetAvailable;
}

bool UsbMidi::waitUntilPacketIsAvailable()
{
    MidiInput unused;
    return receivedMessages.Peek( &unused.input );
}

bool UsbMidi::isPacketAvailable()
{
    return !receivedMessages.IsEmpty();
}

void UsbMidi::discardAllPendingPackets()
{
    receivedMessages.Flush();
}

void UsbMidi::sendControlChange( const uint8_t channel, const uint8_t control, const uint8_t value )
{
    static uint8_t buffer[kMidiPacketSize];
    buffer[0] = kControlChange;
    buffer[1] = (kControlChange << 4) | channel;
    buffer[2] = kControlMask & control;
    buffer[3] = kControlValueMask & value;

    transmitData( buffer, kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOn( const uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    static uint8_t buffer[kMidiPacketSize];
    buffer[0] = kNoteOn;
    buffer[1] = (kNoteOn << 4) | channel;
    buffer[2] = kNoteMask & note;
    buffer[3] = kVelocityMask & velocity;

    transmitData( buffer, kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOff( const uint8_t channel, const uint8_t note )
{
    static uint8_t buffer[kMidiPacketSize];
    buffer[0] = kNoteOff;
    buffer[1] = (kNoteOff << 4) | channel;
    buffer[2] = kNoteMask & note;
    buffer[3] = 0;

    transmitData( buffer, kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendSystemExclussive( const uint8_t* const data, const uint8_t length )
{
    static uint8_t buffer[kMidiPacketSize];
    uint8_t dataIndex = 0;
    while (dataIndex < length)
    {
        const uint8_t bytesRemaining = length - dataIndex;
        switch (bytesRemaining)
        {
            case 3:
                buffer[0] = kSystemExclusiveEnd3Bytes;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = data[dataIndex++];
                break;
            case 2:
                buffer[0] = kSystemExclusiveEnd2Bytes;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = 0;
                break;
            case 1:
                buffer[0] = kSystemExclusiveEnd1Byte;
                buffer[1] = data[dataIndex++];
                buffer[2] = 0;
                buffer[3] = 0;
                break;
            default: // more than 3 bytes left
                buffer[0] = kSystemExclusive;
                buffer[1] = data[dataIndex++];
                buffer[2] = data[dataIndex++];
                buffer[3] = data[dataIndex++];
                break;
        }
        transmitData( buffer, kMidiPacketSize );
        USBD_MIDI_SendPacket();
    }
}

uint16_t UsbMidi::receiveData( uint8_t* const message, const uint16_t length )
{
    const uint16_t numberOfMessages = length / kMidiPacketSize;
    const uint16_t lostBytes = length % kMidiPacketSize;
    if (0 == lostBytes)
    {
        for(uint16_t count = 0; count < numberOfMessages; count++)
        {
            //b4arrq_push(&rxq,((uint32_t *)message)+count);
            BaseType_t unused;
            receivedMessages.EnqueueFromISR( reinterpret_cast<uint32_t*>(message + count*kMidiPacketSize), &unused );
        }
    }
    return 0;
}

uint16_t UsbMidi::transmitData( uint8_t* const message, const uint16_t length )
{
    uint32_t index = 0;
    while (index < length)
    {
        APP_Rx_Buffer[APP_Rx_ptr_in] = *(message + index);
        APP_Rx_ptr_in++;
        index++;
        if (APP_RX_DATA_SIZE == APP_Rx_ptr_in)
        {
            APP_Rx_ptr_in = 0;
        }
    }
    return USBD_OK;
}

} // namespace
