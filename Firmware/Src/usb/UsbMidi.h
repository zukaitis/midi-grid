#ifndef USB_USB_MIDI_H_
#define USB_USB_MIDI_H_

#include <stdint.h>

namespace freertos
{
    class Queue;
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

enum CodeIndexNumber
{
    kMiscellaneousFunctionCodes = 0x00,
    kCableEvents = 0x01,
    kSystemCommon2Bytes = 0x02,
    kSystemCommon3Bytes = 0x03,
    kSystemExclusive = 0x04,
    kSystemExclusiveEnd1Byte = 0x05,
    kSystemExclusiveEnd2Bytes = 0x06,
    kSystemExclusiveEnd3Bytes = 0x07,
    kNoteOff = 0x08,
    kNoteOn = 0x09,
    kPolyKeyPress = 0x0A,
    kControlChange = 0x0B,
    kProgramChange = 0x0C,
    kChannelPressure = 0x0D,
    kPitchBendChange = 0x0E,
    kSingleByte = 0x0F
};

static const uint8_t kCodeIndexNumberMask = 0x0F;
static const uint8_t kChannelMask = 0x0F;

static const int16_t kMinimumControlValue = 0;
static const int16_t kMaximumControlValue = 127;

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

    static uint16_t receiveData( uint8_t* const message, const uint16_t length );
    static uint16_t transmitData( uint8_t* const message, const uint16_t length );

private:
    static freertos::Queue receivedMessages;
};

} // namespace

#endif // USB_USB_MIDI_H_
