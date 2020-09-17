
#include "application/launchpad/LcdGui.hpp"
#include "application/launchpad/Launchpad.hpp"
#include "application/launchpad/Assets.h"
#include "application/launchpad/Images.hpp"

#include "lcd/LcdInterface.h"
#include "lcd/Parameters.h"
#include "lcd/text/Format.h"
#include "types/Color.h"
#include "types/Coordinates.h"
#include "types/Vector.h"
#include <freertos/ticks.hpp>

#include <etl/cstring.h>
#include <etl/to_string.h>

namespace application
{
namespace launchpad
{

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

static const Coordinates statusBarPosition = { 0, topY };
static const uint16_t statusBarHeight = 23;
static const Coordinates modePosition = { 0, 42 };
static const uint16_t modeHeight = 29;
static const Coordinates timingPosition = { 0, 85 };
static const uint16_t timingHeight = 80;
static const Coordinates mixerClipPosition = { 0, 179 };
static const uint16_t mixerClipHeight = 67;
static const Coordinates rotaryPosition = { 0, 260 };
static const uint16_t rotaryHeight = 60;

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
    lcd_.image().createNew( lcd::parameters::width, statusBarHeight );

    lcd_.shapes().drawImage( {5, 0}, lcd::image::triangle_21x23, (launchpad_.isPlaying_ ? playActive : inactive) );
    lcd_.shapes().drawImage( {42, 0}, lcd::image::circle_23x23, (launchpad_.isRecording_ ? recordingActive : inactive) );
    lcd_.shapes().drawImage( {81, 0}, lcd::image::circle_empty_23x23, (launchpad_.isSessionRecording_ ? recordingActive : inactive) );
    lcd_.shapes().drawImage( {174, 0}, lcd::image::usb_41x23,
        ((usbMidiInputActivityIcon_.enabled || usbMidiOutputActivityIcon_.enabled) ? midiActive : inactive) );
    lcd_.shapes().drawImage( {217, 0}, lcd::image::down_arrow_9x23, (usbMidiInputActivityIcon_.enabled ? midiActive : inactive) );
    lcd_.shapes().drawImage( {225, 0}, lcd::image::up_arrow_9x23, (usbMidiOutputActivityIcon_.enabled ? midiActive : inactive) );

    lcd_.image().display( statusBarPosition );
}

void LcdGui::refreshMode()
{
    static const uint16_t radius = modeHeight / 2;
    static const uint16_t textMaxWidth = activeAreaWidth - 2 * (radius + 1);
    static const uint16_t textHeight = 24;
    static const Coordinates textPosition = {centerX, (modeHeight - textHeight) / 2 + 1};
    static const uint16_t topRectangleLowerY = textPosition.y - 1;
    static const uint16_t bottomRectangleUpperY = textPosition.y + textHeight;
    static const uint16_t bottomRectangleLowerY = modeHeight - 1;

    if ((Submode::DEFAULT != launchpad_.submode_) || (Mode::UNKNOWN != launchpad_.mode_))
    {
        etl::string<Launchpad::maximumDawInfoStringLength> text = "";
        Color color = background;
        if (Submode::DEFAULT == launchpad_.submode_)
        {
            // Display device name instead of mode if it is present
            if ((Mode::DEVICE_CONTROLLER == launchpad_.mode_) && (false == launchpad_.deviceName_.empty()))
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
        
        lcd_.image().createNew( lcd::parameters::width, modeHeight, background );

        lcd::Format textFormat;
        textFormat.font( lcd::font::rubik_24p ).textColor( color::WHITE ).backgroundColor( color );
        textFormat.justification( lcd::Justification::CENTER ).maxWidth( textMaxWidth ).abbreviationSuffix( ".." );

        const uint16_t textWidth = lcd_.text().print( text, textPosition, textFormat );
        const uint16_t leftCircleX = centerX - (textWidth / 2) - 1;
        const uint16_t rightCircleX = leftCircleX + textWidth + 1;
        lcd_.shapes().drawHalfCircleLeft( {leftCircleX, radius}, radius, color );
        lcd_.shapes().drawHalfCircleRight( {rightCircleX, radius}, radius, color );
        lcd_.shapes().drawRectangle( {leftCircleX, 0}, {rightCircleX, topRectangleLowerY}, color );
        lcd_.shapes().drawRectangle( {leftCircleX, bottomRectangleUpperY}, {rightCircleX, bottomRectangleLowerY}, color );

        lcd_.image().display( modePosition );
    }
}

void LcdGui::refreshTimingArea()
{
    static const Color timingColor = color::YELLOW;
    static const auto numberFormat = lcd::Format().font( bigFont ).textColor( timingColor );
    static const auto suffixFormat = lcd::Format().textColor( timingColor ).font( smallFont );

    lcd_.image().createNew( lcd::parameters::width, timingHeight , background );

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

    lcd_.image().display( timingPosition );
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
            displayMixerView();
            break;
    }
}

void LcdGui::displayClipView()
{
    static const uint16_t trackRectangleWidth = activeAreaWidth;
    static const uint16_t rectangleHeight = 31;
    static const uint16_t separatorHeight = 5;
    static const uint16_t playingStatusSquareLength = rectangleHeight;
    static const uint16_t playingStatusSeparatorWidth = 3;
    static const uint16_t clipRectangleWidth = trackRectangleWidth - playingStatusSquareLength - playingStatusSeparatorWidth;
    static const uint16_t textMargin = 6;
    static const uint16_t textHeight = 24;
    static const Color emptyColor( 165, 165, 165 );

    static const uint16_t textDisplacementTop = (rectangleHeight - textHeight + 1) / 2;
    static const uint16_t trackTextWidth = trackRectangleWidth - 2 * textMargin;
    static const uint16_t clipTextWidth = clipRectangleWidth - 2 * textMargin;
    static const uint16_t emptyClipSquareLength = (rectangleHeight + 1) / 2;
    static const Coordinates trackRectanglePosition = {leftX, 0};
    static const Coordinates trackTextPosition = trackRectanglePosition + Vector( textMargin, textDisplacementTop );
    static const Coordinates playingStatusSquarePosition = trackRectanglePosition + Vector( 0, rectangleHeight + separatorHeight );
    static const Coordinates clipRectanglePosition = playingStatusSquarePosition +
        Vector( playingStatusSquareLength + playingStatusSeparatorWidth, 0 );
    static const Coordinates clipTextPosition = clipRectanglePosition + Vector( textMargin, textDisplacementTop );
    static const uint16_t emptyClipSquareDisplacement = (playingStatusSquareLength - emptyClipSquareLength) / 2;
    static const Coordinates emptyClipSquarePosition = playingStatusSquarePosition +
        Vector( emptyClipSquareDisplacement, emptyClipSquareDisplacement );
    static const Coordinates playIconPosition = playingStatusSquarePosition +
        Vector( (playingStatusSquareLength - lcd::image::triangle_21x23.getWidth()) / 2,
        (playingStatusSquareLength - lcd::image::triangle_21x23.getHeight()) / 2 );

    lcd::Format format;
    format.font( lcd::font::rubik_24p ).textColor( color::BLACK ).maxWidth( trackTextWidth );

    lcd_.image().createNew( lcd::parameters::width, mixerClipHeight, background );

    lcd_.shapes().drawRectangle( trackRectanglePosition, trackRectangleWidth, rectangleHeight, launchpad_.trackColor_ );
    lcd_.text().print( launchpad_.trackName_, trackTextPosition, format );

    if (launchpad_.hasClip_)
    {
        lcd_.shapes().drawRectangle( clipRectanglePosition, clipRectangleWidth, rectangleHeight, launchpad_.clipColor_ );
        format.maxWidth( clipTextWidth );
        lcd_.text().print( launchpad_.clipName_, clipTextPosition, format );

        // check if clip is playing, once implemented
        lcd_.shapes().drawRectangle( playingStatusSquarePosition, playingStatusSquareLength, playingStatusSquareLength,
            (launchpad_.clipIsPlaying_ ? color::BLACK : launchpad_.clipColor_) );
        lcd_.shapes().drawImage( playIconPosition, lcd::image::triangle_21x23, (launchpad_.clipIsPlaying_ ? playActive : background) );
    }
    else
    {
        lcd_.shapes().drawRectangle( playingStatusSquarePosition, trackRectangleWidth, rectangleHeight, emptyColor );
        lcd_.shapes().drawRectangle( emptyClipSquarePosition, emptyClipSquareLength, emptyClipSquareLength, background );
    }

    lcd_.image().display( mixerClipPosition );
}

void LcdGui::displayMixerView()
{
    lcd::Format format;
    format.font( lcd::font::nokia_8p ).textColor( color::WHITE );

    lcd_.image().createNew( lcd::parameters::width, mixerClipHeight, background );

    lcd_.text().print( launchpad_.deviceName_, {5, 0}, format );
    lcd_.image().display( mixerClipPosition );

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
    
    lcd_.image().createNew( lcd::parameters::width, rotaryHeight, background );

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

    lcd_.image().display( rotaryPosition );
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

}  // namespace launchpad
}  // namespace application
