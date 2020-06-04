#include "lcd/Format.h"
#include "lcd/Font.h"

namespace lcd
{

Format::Format():
    font_( &font::nokia_8p ),
    textColor_( color::WHITE ),
    backgroundColor_( color::BLACK ),
    backgroundColorSet_( false ),
    justification_( Justification::LEFT )
{
}

Format& Format::font( const Font& font )
{
    font_ = &font;
    return *this;
}

Format& Format::textColor( const Color& color )
{
    textColor_ = color;
    return *this;
}

Format& Format::backgroundColor( const Color& color )
{
    backgroundColor_ = color;
    backgroundColorSet_ = true;
    return *this;
}

Format& Format::justification( const Justification justification )
{
    justification_ = justification;
    return *this;
}

Format& Format::maxWidth( const uint16_t width )
{
    maxWidth_ = width;
    return *this;
}

const Font& Format::font() const
{
    return *font_;
}

const Color& Format::textColor() const
{
    return textColor_;
}

const Color& Format::backgroundColor() const
{
    return backgroundColor_;
}

bool Format::isBackgroundColorSet() const
{
    return backgroundColorSet_;
}

Justification Format::justification() const
{
    return justification_;
}

uint16_t Format::maxWidth() const
{
    return maxWidth_;
}

}  // namespace lcd
