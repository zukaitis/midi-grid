
#include "application/launchpad/LcdGui.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/launchpad/Assets.h"
#include "application/launchpad/Images.hpp"

#include "lcd/Format.h"
#include "lcd/LcdInterface.h"
#include "lcd/Font.h"
#include <freertos/ticks.hpp>

#include <etl/cstring.h>
#include <etl/to_string.h>

namespace application
{
namespace launchpad
{

static const lcd::ImageLegacy usbSymbolSmall = { &usbSymbolSmallArray[0], 9, 8 };
static const lcd::ImageLegacy midiSymbolSmall = { &midiSymbolSmallArray[0], 8, 8 };
static const lcd::ImageLegacy arrowSmallUp = { &arrowSmallUpArray[0], 5, 8 };
static const lcd::ImageLegacy arrowSmallDown = { &arrowSmallDownArray[0], 5, 8 };
static const lcd::ImageLegacy play = { &playArray[0], 16, 16 };
static const lcd::ImageLegacy recordingOn = { &recordingOnArray[0], 16, 16 };
static const lcd::ImageLegacy sessionRecordingOn = { &sessionRecordingOnArray[0], 16, 16 };
static const lcd::ImageLegacy nudgeDownInactive = { &nudgeDownInactiveArray[0], 10, 8};
static const lcd::ImageLegacy nudgeDownActive = { &nudgeDownActiveArray[0], 10, 8};
static const lcd::ImageLegacy nudgeUpInactive = { &nudgeUpInactiveArray[0], 10, 8};
static const lcd::ImageLegacy nudgeUpActive = { &nudgeUpActiveArray[0], 10, 8};

static const uint32_t kMidiActivityTimeoutMs = 1000;
static const uint32_t kRotaryControlTimeoutMs = 1000;

static const Color background = {110, 110, 110};
static const Color playActive = {10, 255, 128};
static const Color recordingActive = {255, 54, 64};
static const Color midiActive = {255, 181, 50};
static const Color inactive = color::BLACK;

static const lcd::Font& smallFont = lcd::font::rubik_24p;
static const lcd::Font& bigFont = lcd::font::monoton_80p;

static const uint16_t leftX = 5;
static const uint16_t rightX = 234;
static const uint16_t centerX = 120;
static const uint16_t topY = 5;
static const uint16_t bottomY = 314;
static const uint16_t activeAreaWidth = rightX - leftX + 1;
static const uint16_t activeAreaHeight = bottomY - topY + 1;

LcdGui::LcdGui( Launchpad* launchpad, lcd::LcdInterface* lcd ):
    launchpad_( *launchpad ),
    lcd_( *lcd )
{
}

void LcdGui::initialize()
{
    lcd_.setBackgroundColor( background );
    lcd_.clear();
    // displayLaunchpad95Info();
}

void LcdGui::refresh()
{
    refreshTimedItemsStatus();
    refreshStatusBar();
    refreshMode();
    refreshTimingArea();
    refreshModeDependentArea();
    refreshRotaryControlArea();
}

void LcdGui::refreshStatusBar()
{
    lcd_.displayImage( {leftX, topY}, lcd::image::triangle_21x23, (launchpad_.isPlaying_ ? playActive : inactive) );
    lcd_.displayImage( {42, topY}, lcd::image::circle_23x23, (launchpad_.isRecording_ ? recordingActive : inactive) );
    lcd_.displayImage( {81, topY}, lcd::image::circle_empty_23x23, (launchpad_.isSessionRecording_ ? recordingActive : inactive) );

    lcd_.displayImage( {174, topY}, lcd::image::usb_41x23,
        ((usbMidiInputActivityIcon_.enabled || usbMidiOutputActivityIcon_.enabled) ? midiActive : inactive) );
    lcd_.displayImage( {217, topY}, lcd::image::down_arrow_9x23, (usbMidiInputActivityIcon_.enabled ? midiActive : inactive) );
    lcd_.displayImage( {226, topY}, lcd::image::up_arrow_9x23, (usbMidiOutputActivityIcon_.enabled ? midiActive : inactive) );
}

void LcdGui::refreshMode()
{
    static const uint16_t radius = 12;

    if ((Submode::DEFAULT != launchpad_.submode_) || (Mode::UNKNOWN != launchpad_.mode_))
    {
        etl::string<20> text = "";
        Color color = background;
        if (Submode::DEFAULT == launchpad_.submode_)
        {
            // Display device name instead of mode if it is present
            if ((Mode::DEVICE_CONTROLLER == launchpad_.mode_) && (0 != launchpad_.deviceName_.length()))
            {
                text = launchpad_.deviceName_;
            }
            else
            {
                text = modeAttributes.at( launchpad_.mode_ ).displayString;
            }
            color = modeAttributes.at( launchpad_.mode_ ).color;
        }
        else
        {
            text = submodeAttributes.at( launchpad_.submode_ ).displayString;
            color = submodeAttributes.at( launchpad_.submode_ ).color;
        }
        
        lcd::Format textFormat;
        textFormat.font( lcd::font::rubik_24p ).textColor( color::WHITE ).backgroundColor( color );
        const uint16_t textWidth = textFormat.font().getStringWidth( text );
        const uint16_t textHeight = textFormat.font().getHeight();
        const Coordinates textStart = {static_cast<uint16_t>(centerX - textWidth / 2), 32};

        lcd_.print( text, textStart, textFormat );
        lcd_.draw().halfCircleLeft( {static_cast<uint16_t>(textStart.x - 1),
            static_cast<uint16_t>(textStart.y + radius)}, radius, color );
        lcd_.draw().halfCircleRight( {static_cast<uint16_t>(textStart.x + textWidth),
            static_cast<uint16_t>(textStart.y + radius)}, radius, color );
        lcd_.fillArea({textStart.x, static_cast<uint16_t>(textStart.y + textHeight)},
            {static_cast<uint16_t>(textStart.x + textWidth), static_cast<uint16_t>(textStart.y + textHeight)}, color );
        lcd_.clearArea( {0, textStart.y}, {static_cast<uint16_t>(textStart.x - radius - 1),
            static_cast<uint16_t>(textStart.y + textHeight + 1)} );
        lcd_.clearArea( {239, textStart.y}, {static_cast<uint16_t>(textStart.x + textWidth + radius + 1),
            static_cast<uint16_t>(textStart.y + textHeight + 1)} );
    }
}

void LcdGui::refreshTimingArea()
{
    static const uint16_t timingTopY = 64;
    static const uint16_t timingBottomY = 143;
    static const Color timingColor = color::YELLOW;
    static const auto numberFormat = lcd::Format().font( bigFont ).textColor( timingColor );
    static const auto textFormat = lcd::Format().textColor( timingColor ).font( smallFont );

    if (0 != launchpad_.tempo_)
    {
        etl::string<3> numberString = {};
        etl::to_string( launchpad_.tempo_, numberString );

        const uint16_t numberWidth = numberFormat.font().getStringWidth( numberString );
        uint16_t textWidth = textFormat.font().getStringWidth( " bpm" );
        const uint16_t textHeight = textFormat.font().getHeight();
        const bool showText = ((numberWidth + textWidth) <= activeAreaWidth);

        if (false == showText)
        {
            textWidth = 0;
        }
        const uint16_t totalWidth = numberWidth + textWidth;
        const uint16_t numberLeftX = centerX - totalWidth / 2;

        lcd_.clearArea( {0, timingTopY}, {static_cast<uint16_t>(numberLeftX - 1U), timingBottomY} );
        lcd_.print( numberString, {numberLeftX, timingTopY}, numberFormat );

        if (showText)
        {
            lcd_.print( " bpm", {static_cast<uint16_t>(numberLeftX + numberWidth), timingTopY}, textFormat );
        }

        lcd_.clearArea( {static_cast<uint16_t>(numberLeftX + totalWidth), timingTopY},
            {239, static_cast<uint16_t>(timingTopY + textHeight - 1U)} );
        lcd_.clearArea( {static_cast<uint16_t>(numberLeftX + numberWidth), static_cast<uint16_t>(timingTopY + textHeight)},
            {239, timingBottomY} );
    }
    else
    {
        lcd_.clearArea( {0, timingTopY}, {239, timingBottomY} );
    }
}

void LcdGui::refreshModeDependentArea()
{
    switch (launchpad_.mode_)
    {
        case Mode::INSTRUMENT:
        case Mode::DRUM_STEP_SEQUENCER:
        case Mode::MELODIC_SEQUENCER:
        case Mode::DEVICE_CONTROLLER:
            displayClipView();
        case Mode::SESSION:
        case Mode::MIXER:
        case Mode::USER1:
        case Mode::USER2:
        default:
            // think of a name for this view later
            break;
    }
}

void LcdGui::displayClipView()
{
    
}

void LcdGui::refreshRotaryControlArea()
{
    static const uint16_t minAngle = 45;
    static const uint16_t maxAngle = 315;
    static const Color color = {114, 206, 243};
    static const uint16_t innerRadius = 16;
    static const uint16_t outerRadius = 33;
    static const uint16_t centerY = 290;
    static const uint16_t textY = 290;
    lcd::Format textFormat;
    textFormat.font( lcd::font::rubik_24p ).textColor( color );
    
    uint16_t angle = minAngle + ((maxAngle - minAngle) * launchpad_.rotaryControlValue_.at( 0 ) ) / midi::kMaximumControlValue;
    lcd_.draw().arc( {leftX + outerRadius, centerY}, innerRadius, outerRadius, minAngle, angle, color );
    etl::string<4> stringControl0 = " ";
    etl::to_string( launchpad_.rotaryControlValue_.at( 0 ), stringControl0, true );
    lcd_.print( stringControl0, {leftX + 2 * outerRadius, textY}, textFormat );

    angle = minAngle + ((maxAngle - minAngle) * launchpad_.rotaryControlValue_.at( 1 ) ) / midi::kMaximumControlValue;
    lcd_.draw().arc( {rightX - outerRadius, centerY}, innerRadius, outerRadius, minAngle, angle, color );
    etl::string<4> stringControl1;
    etl::to_string( launchpad_.rotaryControlValue_.at( 1 ), stringControl1 );
    stringControl1 += " ";
    lcd_.print( stringControl1, {rightX - 2 * outerRadius, textY}, textFormat.justification( lcd::Justification::RIGHT ) );

    lcd_.clearArea( {static_cast<uint16_t>(leftX + 2 * outerRadius + textFormat.font().getStringWidth( stringControl0 ))},
        {static_cast<uint16_t>(rightX - 2 * outerRadius - textFormat.font().getStringWidth( stringControl1 )), bottomY} );
}

void LcdGui::refreshMainArea()
{

    displayLaunchpad95Info();
}

void LcdGui::refreshTimedItemsStatus()
{
    if (usbMidiInputActivityIcon_.enabled)
    {
        if (freertos::Ticks::GetTicks() >= usbMidiInputActivityIcon_.timeToDisable)
        {
            usbMidiInputActivityIcon_.enabled = false;
        }
    }

    if (usbMidiOutputActivityIcon_.enabled)
    {
        if (freertos::Ticks::GetTicks() >= usbMidiOutputActivityIcon_.timeToDisable)
        {
            usbMidiOutputActivityIcon_.enabled = false;
        }
    }
}

void LcdGui::registerMidiInputActivity()
{
    usbMidiInputActivityIcon_.enabled = true;
    usbMidiInputActivityIcon_.timeToDisable = freertos::Ticks::GetTicks() + kMidiActivityTimeoutMs;
}


void LcdGui::registerMidiOutputActivity()
{
    usbMidiOutputActivityIcon_.enabled = true;
    usbMidiOutputActivityIcon_.timeToDisable = freertos::Ticks::GetTicks() + kMidiActivityTimeoutMs;
}

void LcdGui::displayLaunchpad95Info()
{
    if (Submode::DEFAULT == launchpad_.submode_)
    {
        displayMode();
    }
    else
    {
        displaySubmode();
    }

    // only display other info when rotary control display timer runs out
        lcd_.clearArea( {0, 16}, {83, 31} );
        //displayStatus();

        lcd_.clearArea( {0, 32}, {83, 47} );
        switch (launchpad_.mode_)
        {
            case Mode::INSTRUMENT:
            case Mode::DRUM_STEP_SEQUENCER:
            case Mode::MELODIC_SEQUENCER:
                displayTrackName();
                displayClipName();
                break;
            case Mode::DEVICE_CONTROLLER:
                displayTrackName();
                displayDeviceName();
                break;
            case Mode::SESSION:
            case Mode::MIXER:
            case Mode::USER1:
            case Mode::USER2:
            default:
                //displayTimingStatus();
                break;
        }
}

void LcdGui::displayClipName()
{
    lcd_.print( &launchpad_.clipName_[0], lcd_.line( 5 ), lcd::Justification::CENTER );
}

void LcdGui::displayDeviceName()
{
    lcd_.print( &launchpad_.deviceName_[0], lcd_.line( 5 ), lcd::Justification::CENTER );
}

void LcdGui::displayTrackName()
{
    lcd_.print( &launchpad_.trackName_[0], lcd_.line( 4 ), lcd::Justification::CENTER );
}

void LcdGui::displayMode()
{
    lcd_.clearArea( {0, 8}, {83, 15} );
    if (Mode::UNKNOWN != launchpad_.mode_)
    {
        //lcd_.print( &launchpad95ModeString.at(launchpad_.mode_)[0], lcd_.line( 1 ), lcd::Justification::CENTER );
    }
}

void LcdGui::displaySubmode()
{
    lcd_.clearArea( {0, 8}, {83, 15} );
    //lcd_.print( &launchpad95SubmodeString.at(launchpad_.submode_)[0], lcd_.line( 1 ), lcd::Justification::CENTER );
}

}  // namespace launchpad
}  // namespace application
