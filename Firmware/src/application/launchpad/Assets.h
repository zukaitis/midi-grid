#include "application/launchpad/Launchpad.hpp"
#include "types/Color.h"
#include "types/Coordinates.h"
#include <etl/array.h>
#include <etl/cstring.h>
#include <etl/string_view.h>
#include <etl/map.h>

namespace application
{
namespace launchpad
{

/* MIDI */
const uint8_t kControlValueLow = 0;
const uint8_t kControlValueHigh = 127;

const etl::array<uint8_t, 10> kChallengeResponse = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x18, 0x40, 0x00, 0x00, 0xF7 };

const etl::string<6> kStandardSystemExclussiveMessageHeader = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x02, 0x18 };
const etl::string_view standardSystemExclussiveMessageHeaderView( kStandardSystemExclussiveMessageHeader );
const uint8_t kStandardSystemExclussiveMessageMinimumLength = 8;

const etl::string<9> systemExclussiveBootloaderMessage = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x00, 0x71, 0x00, 0x69,
    static_cast<char>(0xF7) };
const etl::string_view systemExclussiveBootloaderMessageView( systemExclussiveBootloaderMessage );

enum class LaunchpadSysExCommand
{
    SET_LED = 0x0A,
    SET_LED_RGB = 0x0B,
    SET_LEDS_BY_COLUMN = 0x0C,
    SET_LEDS_BY_ROW = 0x0D,
    SET_ALL_LEDS = 0x0E,
    TEXT_SCROLL = 0x14,
    SET_LAYOUT = 0x22,
    FLASH_LED = 0x23,
    PULSE_LED = 0x28,
    FADER_SETUP = 0x2B,
    DISCONNECT_OR_CHALLENGE = 0x40
};

const etl::string<6> customSysExMessageHeader = { static_cast<char>(0xF0), 0x00, 0x20, 0x29, 0x02, 0x07 };
const etl::string_view customSysExMessageHeaderView( customSysExMessageHeader );

enum class CustomSysExCommand
{
    INJECT_BUTTON_PRESS = 0
};

enum class MidiChannel : uint8_t
{
    SESSION_LAYOUT = 0,
    DEVICE_CONTROL = 0,
    ADDITIONAL_CONTROL = 5,
    USER1_LAYOUT = 7, // can select channel from 6, 7 and 8
    USER2_LAYOUT = 15 // can select channel from 14, 15 and 16
};

const uint8_t kAdditionalNoteButtonNote = 55;

/* Grid */
const uint8_t kNumberOfColumns = 10;
const uint8_t kNumberOfRows = 8;

const uint8_t kSubmodeColumn = 8;
const uint8_t kDeviceControlColumn = 9;

struct DeviceControlButton
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t controlValue;
};

const DeviceControlButton kSession = {.positionX = 9, .positionY = 3, .controlValue = 108};
const DeviceControlButton kMixer = {.positionX = 9, .positionY = 1, .controlValue = 111};
const DeviceControlButton kUser1 = {.positionX = 9, .positionY = 2, .controlValue = 109};
const DeviceControlButton kUser2 = {.positionX = 9, .positionY = 0, .controlValue = 110};
const DeviceControlButton kUp = {.positionX = 9, .positionY = 4, .controlValue = 104};
const DeviceControlButton kDown = {.positionX = 9, .positionY = 7, .controlValue = 105};
const DeviceControlButton kLeft = {.positionX = 9, .positionY = 5, .controlValue = 106};
const DeviceControlButton kRight = {.positionX = 9, .positionY = 6, .controlValue = 107};

const uint8_t kMinimumDeviceControlValue = kUp.controlValue;
const uint8_t kMaximumDeviceControlValue = kMixer.controlValue;

// buttons are numerated bottom to top, same as in grid
const etl::array<uint8_t, kNumberOfRows> kDeviceControlColumnValue = {
        kUser2.controlValue, kMixer.controlValue, kUser1.controlValue, kSession.controlValue,
        kUp.controlValue, kLeft.controlValue, kRight.controlValue, kDown.controlValue };

const etl::array<etl::array<uint8_t, kNumberOfRows>, kNumberOfColumns> kSessionLayout = {{
        {11, 21, 31, 41, 51, 61, 71, 81}, {12, 22, 32, 42, 52, 62, 72, 82},
        {13, 23, 33, 43, 53, 63, 73, 83}, {14, 24, 34, 44, 54, 64, 74, 84},
        {15, 25, 35, 45, 55, 65, 75, 85}, {16, 26, 36, 46, 56, 66, 76, 86},
        {17, 27, 37, 47, 57, 67, 77, 87}, {18, 28, 38, 48, 58, 68, 78, 88},
        {19, 29, 39, 49, 59, 69, 79, 89}, {110, 111, 109, 108, 104, 106, 107, 105} }};

const etl::array<etl::array<uint8_t, kNumberOfRows>, kNumberOfColumns> kDrumLayout = {{
        {36, 40, 44, 48, 52, 56, 60, 64}, {37, 41, 45, 49, 53, 57, 61, 65},
        {38, 42, 46, 50, 54, 58, 62, 66}, {39, 43, 47, 51, 55, 59, 63, 67},
        {68, 72, 76, 80, 84, 88, 92, 96}, {69, 73, 77, 81, 85, 89, 93, 97},
        {70, 74, 78, 82, 86, 90, 94, 98}, {71, 75, 79, 83, 87, 91, 95, 99},
        {107, 106, 105, 104, 103, 102, 101, 100}, {110, 111, 109, 108, 104, 106, 107, 105} }};

const etl::array<Color, 128> palette = {
    Color(0, 0, 0), Color(28, 28, 28), Color(124, 124, 124), Color(252, 252, 252), Color(255, 77, 71), Color(255, 10, 0), Color(90, 1, 0),
    Color(24, 0, 0), Color(255, 189, 98), Color(255, 86, 0), Color(90, 29, 0), Color(36, 24, 0), Color(253, 253, 33), Color(253, 253, 0),
    Color(88, 88, 0), Color(23, 23, 0), Color(128, 253, 42), Color(64, 253, 0), Color(22, 88, 0), Color(19, 40, 0), Color(52, 253, 43),
    Color(0, 253, 0), Color(0, 88, 0), Color(0, 24, 0), Color(51, 253, 70), Color(0, 253, 0), Color(0, 88, 0), Color(0, 24, 0),
    Color(50, 253, 126), Color(0, 253, 58), Color(0, 88, 20), Color(0, 28, 15), Color(47, 252, 176), Color(0, 252, 145), Color(0, 88, 49),
    Color(0, 24, 15), Color(57, 191, 255), Color(0, 167, 255), Color(0, 64, 81), Color(0, 16, 24), Color(65, 135, 255), Color(0, 80, 255),
    Color(0, 26, 90), Color(0, 7, 25), Color(70, 71, 255), Color(0, 0, 255), Color(0, 0, 90), Color(0, 0, 25), Color(131, 71, 255),
    Color(80, 0, 255), Color(22, 0, 103), Color(11, 0, 50), Color(255, 73, 255), Color(255, 0, 255), Color(90, 0, 90), Color(25, 0, 25),
    Color(255, 77, 132), Color(255, 7, 82), Color(90, 1, 27), Color(33, 0, 16), Color(255, 25, 0), Color(155, 53, 0), Color(122, 81, 0),
    Color(62, 100, 0), Color(0, 56, 0), Color(0, 84, 50), Color(0, 83, 126), Color(0, 0, 255), Color(0, 68, 77), Color(27, 0, 210),
    Color(124, 124, 124), Color(32, 32, 32), Color(255, 10, 0), Color(186, 253, 0), Color(170, 237, 0), Color(86, 253, 0), Color(0, 136, 0),
    Color(0, 252, 122), Color(0, 167, 255), Color(0, 27, 255), Color(53, 0, 255), Color(119, 0, 255), Color(180, 23, 126), Color(65, 32, 0),
    Color(255, 74, 0), Color(131, 225, 0), Color(101, 253, 0), Color(0, 253, 0), Color(0, 253, 0), Color(69, 253, 97), Color(0, 252, 202),
    Color(80, 134, 255), Color(39, 77, 201), Color(130, 122, 237), Color(211, 12, 255), Color(255, 6, 90), Color(255, 125, 0),
    Color(185, 177, 0), Color(138, 253, 0), Color(130, 93, 0), Color(53, 40, 0), Color(13, 76, 5), Color(0, 80, 55), Color(19, 19, 41),
    Color(16, 31, 90), Color(106, 60, 23), Color(172, 4, 0), Color(255, 81, 53), Color(220, 105, 0), Color(255, 255, 0), Color(153, 225, 0),
    Color(95, 181, 0), Color(27, 27, 49), Color(220, 253, 81), Color(118, 252, 184), Color(150, 151, 255), Color(139, 97, 255),
    Color(64, 64, 64), Color(116, 116, 116), Color(222, 252, 252), Color(164, 4, 0), Color(53, 0, 0), Color(0, 209, 0), Color(0, 64, 0),
    Color(185, 177, 0), Color(61, 48, 0), Color(180, 93, 0), Color(74, 20, 0) };

struct ModeAttributes
{
    etl::string<20> displayString;
    Color color;
    Coordinates definingLed;
};

const etl::map<Mode, ModeAttributes, 8> modeAttributes = {{
    {Mode::SESSION, {"Session", palette.at(21), {9, 3}}},
    {Mode::INSTRUMENT, {"Instrument", palette.at(37), {9, 2}}},
    {Mode::DEVICE_CONTROLLER, {"Device control", palette.at(48), {9, 2}}},
    {Mode::USER1, {"User 1", palette.at(45), {9, 2}}},
    {Mode::DRUM_STEP_SEQUENCER, {"Drum sequencer", palette.at(53), {9, 0}}},
    {Mode::MELODIC_SEQUENCER, {"Melodic sequencer", palette.at(9), {9, 0}}},
    {Mode::USER2, {"User 2", palette.at(45), {9, 0}}},
    {Mode::MIXER, {"Mixer", palette.at(29), {9, 1}}}
}};

struct SubmodeAttributes
{
    Mode mode;
    etl::string<20> displayString;
    Color color;
    Coordinates definingLed;
};

const etl::map<Submode, SubmodeAttributes, 10> submodeAttributes = {{
    {Submode::DEFAULT, {Mode::UNKNOWN, " ", Color(255, 255, 255), {10, 10}}}, // non-existant coordinates
    {Submode::SCALE_INSTRUMENT, {Mode::INSTRUMENT, "Scale", palette.at(9), {8, 7}}},
    {Submode::SCALE, {Mode::DRUM_STEP_SEQUENCER, "Scale", palette.at(5), {8, 7}}},
    {Submode::VOLUME, {Mode::MIXER, "Volume", palette.at(31), {8, 7}}},
    {Submode::PAN, {Mode::MIXER, "Pan", palette.at(31), {8, 6}}},
    {Submode::SEND_A, {Mode::MIXER, "Send A", palette.at(31), {8, 5}}},
    {Submode::SEND_B, {Mode::MIXER, "Send B", palette.at(31), {8, 4}}},
    {Submode::LENGTH, {Mode::MELODIC_SEQUENCER, "MSS: Length", palette.at(29), {8, 3}}},
    {Submode::OCTAVE, {Mode::MELODIC_SEQUENCER, "MSS: Octave", palette.at(48), {8, 2}}},
    {Submode::VELOCITY, {Mode::MELODIC_SEQUENCER, "MSS: Velocity", palette.at(37), {8, 1}}}
}};

}  // namespace launchpad
}  // namespace application
