
#include "application/launchpad/LcdGui.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/launchpad/Assets.h"
#include "application/launchpad/Images.hpp"

#include "lcd/LcdInterface.h"
#include "lcd/Parameters.h"
#include "lcd/text/Format.h"
#include "types/Color.h"
#include "types/Coordinates.h"
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

static const uint16_t margin = 5;
static const uint16_t leftX = margin;
static const uint16_t rightX = lcd::parameters::width - margin;
static const uint16_t centerX = lcd::parameters::width / 2;
static const uint16_t topY = margin;
static const uint16_t bottomY = lcd::parameters::height - margin;
static const uint16_t activeAreaWidth = rightX - leftX;
static const uint16_t activeAreaHeight = bottomY - topY;

LcdGui::LcdGui( Launchpad* launchpad, lcd::LcdInterface* lcd ):
    launchpad_( *launchpad ),
    lcd_( *lcd )
{
}

void LcdGui::initialize()
{
    lcd_.image().createNew( background );
    lcd_.image().display();
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
    lcd_.image().createNew( 230, 23 );

    lcd_.shapes().drawImage( {0, 0}, lcd::image::triangle_21x23, (launchpad_.isPlaying_ ? playActive : inactive) );
    lcd_.shapes().drawImage( {37, 0}, lcd::image::circle_23x23, (launchpad_.isRecording_ ? recordingActive : inactive) );
    lcd_.shapes().drawImage( {76, 0}, lcd::image::circle_empty_23x23, (launchpad_.isSessionRecording_ ? recordingActive : inactive) );
    lcd_.shapes().drawImage( {169, 0}, lcd::image::usb_41x23,
        ((usbMidiInputActivityIcon_.enabled || usbMidiOutputActivityIcon_.enabled) ? midiActive : inactive) );
    lcd_.shapes().drawImage( {212, 0}, lcd::image::down_arrow_9x23, (usbMidiInputActivityIcon_.enabled ? midiActive : inactive) );
    lcd_.shapes().drawImage( {221, 0}, lcd::image::up_arrow_9x23, (usbMidiOutputActivityIcon_.enabled ? midiActive : inactive) );

    lcd_.image().display( {leftX, topY} );
}

void LcdGui::refreshMode()
{
    static const uint16_t radius = 12;
    static const uint16_t textMaxWidth = activeAreaWidth - 2 * (radius + 1);

    if ((Submode::DEFAULT != launchpad_.submode_) || (Mode::UNKNOWN != launchpad_.mode_))
    {
        etl::string<Launchpad::maximumDawInfoStringLength> text = "";
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
        
        lcd_.image().createNew( lcd::parameters::width, 25, background );

        lcd::Format textFormat;
        textFormat.font( lcd::font::rubik_24p ).textColor( color::WHITE ).backgroundColor( color );
        textFormat.justification( lcd::Justification::CENTER ).maxWidth( textMaxWidth ).abbreviationSuffix( ".." );

        const uint16_t textWidth = lcd_.text().print( text, {centerX, 0}, textFormat );
        lcd_.shapes().drawHalfCircleLeft( {static_cast<uint16_t>(centerX - (textWidth / 2) - 1),
            radius}, radius, color );
        lcd_.shapes().drawHalfCircleRight( {static_cast<uint16_t>(centerX + (textWidth / 2)),
            radius}, radius, color );
        lcd_.shapes().drawLine( {static_cast<uint16_t>(centerX - (textWidth / 2) - 1), 24},
            {static_cast<uint16_t>(centerX + (textWidth / 2)), 24}, color );

        lcd_.image().display( {0, 32} );
    }
}

void LcdGui::refreshTimingArea()
{
    static const uint16_t timingTopY = 64;
    static const Color timingColor = color::YELLOW;
    static const auto numberFormat = lcd::Format().font( bigFont ).textColor( timingColor );
    static const auto suffixFormat = lcd::Format().textColor( timingColor ).font( smallFont );

    lcd_.image().createNew( 240, 80 , background );

    if (0 != launchpad_.tempo_)
    {
        etl::string<3> numberString = {};
        etl::to_string( launchpad_.tempo_, numberString );

        const uint16_t numberWidth = numberFormat.font().getStringWidth( numberString );
        uint16_t suffixWidth = suffixFormat.font().getStringWidth( " bpm" );
        const bool displaySuffix = ((numberWidth + suffixWidth) <= activeAreaWidth);

        if (false == displaySuffix)
        {
            suffixWidth = 0;
        }

        const uint16_t totalWidth = numberWidth + suffixWidth;
        const uint16_t numberLeftX = centerX - totalWidth / 2;

        lcd_.text().print( numberString, {numberLeftX, 0}, numberFormat );

        if (displaySuffix)
        {
            lcd_.text().print( " bpm", {static_cast<uint16_t>(numberLeftX + numberWidth), 0}, suffixFormat );
        }
    }

    lcd_.image().display( {0, timingTopY} );
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
    static const uint16_t minAngle = 30;
    static const uint16_t maxAngle = 330;
    static const uint16_t gapAngle = 16;
    static const Color color( 114, 206, 243 );
    static const Color inactive = color::BLACK;
    static const uint16_t innerRadius = 25;
    static const uint16_t outerRadius = 29;
    static const uint16_t lineLength = (innerRadius + outerRadius) / 2;
    static const uint16_t lineThickness = 5;
    static const uint16_t centerY = outerRadius;
    static const uint16_t textY = outerRadius;
    lcd::Format textFormat;
    textFormat.font( lcd::font::rubik_24p ).textColor( color );
    
    lcd_.image().createNew( 240, 64, background );

    uint16_t angle = minAngle + ((maxAngle - minAngle) * launchpad_.rotaryControlValue_.at( 0 ) ) / midi::kMaximumControlValue;
    Coordinates center = {leftX + outerRadius, centerY};
    lcd_.shapes().drawArc( center, innerRadius, outerRadius, minAngle, angle, color );
    lcd_.shapes().drawArc( center, innerRadius, outerRadius, angle + gapAngle, maxAngle, inactive );
    lcd_.shapes().drawLine( center, angle, lineLength, lineThickness, inactive );
    etl::string<4> stringControl0 = " ";
    etl::to_string( launchpad_.rotaryControlValue_.at( 0 ), stringControl0, true );
    lcd_.text().print( stringControl0, {leftX + 2 * outerRadius, textY}, textFormat );

    angle = minAngle + ((maxAngle - minAngle) * launchpad_.rotaryControlValue_.at( 1 ) ) / midi::kMaximumControlValue;
    center = {rightX - outerRadius, centerY};
    lcd_.shapes().drawArc( center, innerRadius, outerRadius, minAngle, angle, color );
    lcd_.shapes().drawArc( center, innerRadius, outerRadius, angle + gapAngle, maxAngle, inactive );
    lcd_.shapes().drawLine( center, angle, lineLength, lineThickness, inactive );
    etl::string<4> stringControl1;
    etl::to_string( launchpad_.rotaryControlValue_.at( 1 ), stringControl1 );
    stringControl1 += " ";
    lcd_.text().print( stringControl1, {rightX - 2 * outerRadius, textY}, textFormat.justification( lcd::Justification::RIGHT ) );

    lcd_.image().display( {0, 256} );
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

#if 0
    // only display other info when rotary control display timer runs out
        lcd_.clearArea( {0, 16}, {83, 31} );
        //displayStatus();

        lcd_.clearArea( {0, 32}, {83, 47} );
#endif
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
    //lcd_.text().print( &launchpad_.clipName_[0], lcd_.line( 5 ), lcd::Format().justification( lcd::Justification::CENTER ) );
}

void LcdGui::displayDeviceName()
{
    //lcd_.text().print( &launchpad_.deviceName_[0], lcd_.line( 5 ), lcd::Format().justification( lcd::Justification::CENTER ) );
}

void LcdGui::displayTrackName()
{
    //lcd_.text().print( &launchpad_.trackName_[0], lcd_.line( 4 ), lcd::Format().justification( lcd::Justification::CENTER ) );
}

void LcdGui::displayMode()
{
    //lcd_.clearArea( {0, 8}, {83, 15} );
    if (Mode::UNKNOWN != launchpad_.mode_)
    {
        //lcd_.text().print( &launchpad95ModeString.at(launchpad_.mode_)[0], lcd_.line( 1 ), lcd::Format().justification( lcd::Justification::CENTER ) );
    }
}

void LcdGui::displaySubmode()
{
    //lcd_.clearArea( {0, 8}, {83, 15} );
    //lcd_.text().print( &launchpad95SubmodeString.at(launchpad_.submode_)[0], lcd_.line( 1 ), lcd::Format().justification( lcd::Justification::CENTER ) );
}

}  // namespace launchpad
}  // namespace application
