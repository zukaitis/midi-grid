#include "usb/UsbMidi.hpp"

#include "stm32f4xx_hal.h"
#include <freertos/queue.hpp>

#include "usbd_midi.h"
#include "usbd_desc.h"

#include <etl/array.h>

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

freertos::Queue UsbMidi::receivedMessages = freertos::Queue( 256, sizeof( MidiPacket ) );

UsbMidi::UsbMidi() = default;

UsbMidi::~UsbMidi() = default;

bool UsbMidi::waitForPacket( MidiPacket* packet )
{
    const bool packetAvailable = receivedMessages.Dequeue( packet ); // block until event
    return packetAvailable;
}

bool UsbMidi::waitUntilPacketIsAvailable()
{
    MidiPacket unused = {};
    return receivedMessages.Peek( &unused );
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
    static etl::array<uint8_t, 4> buffer;
    buffer[0] = kControlChange;
    buffer[1] = (kControlChange << 4U) | channel;
    buffer[2] = kControlMask & control;
    buffer[3] = kControlValueMask & value;

    transmitData( &buffer[0], kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOn( const uint8_t channel, const uint8_t note, const uint8_t velocity )
{
    static etl::array<uint8_t, 4> buffer;
    buffer[0] = kNoteOn;
    buffer[1] = (kNoteOn << 4U) | channel;
    buffer[2] = kNoteMask & note;
    buffer[3] = kVelocityMask & velocity;

    transmitData( &buffer[0], kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendNoteOff( const uint8_t channel, const uint8_t note )
{
    static etl::array<uint8_t, 4> buffer;
    buffer[0] = kNoteOff;
    buffer[1] = (kNoteOff << 4) | channel;
    buffer[2] = kNoteMask & note;
    buffer[3] = 0;

    transmitData( &buffer[0], kMidiPacketSize );
    USBD_MIDI_SendPacket();
}

void UsbMidi::sendSystemExclussive( const uint8_t* const data, const uint8_t length )
{
    static etl::array<uint8_t, 4> buffer;
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
        transmitData( &buffer[0], kMidiPacketSize );
        USBD_MIDI_SendPacket();
    }
}

uint16_t UsbMidi::receiveData( uint8_t* const message, const uint16_t length )
{
    const uint16_t numberOfPackets = length / kMidiPacketSize;
    const uint16_t lostBytes = length % kMidiPacketSize;
    if (0 == lostBytes)
    {
        for (uint16_t packetIndex = 0; packetIndex < numberOfPackets; packetIndex++)
        {
            //b4arrq_push(&rxq,((uint32_t *)message)+packetIndex);
            BaseType_t unused;
            MidiPacket packet = { message[4*packetIndex], {
                message[4*packetIndex+1],
                message[4*packetIndex+2],
                message[4*packetIndex+3] } };
            receivedMessages.EnqueueFromISR( &packet, &unused );
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
