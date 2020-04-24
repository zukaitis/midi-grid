#include "lcd/Format.h"
#include "lcd/Font.h"

namespace lcd
{

Format::Format():
    font_( &font::nokia_8p ),
    textColor_( color::WHITE ),
    backgroundColor_( color::BLACK )
{
}

Format& Format::font( const Font& font )
{
    font_ = &font;
    return *this;
}

Format& Format::textColor( const Pixel& color )
{
    textColor_ = color;
    return *this;
}

Format& Format::backgroundColor( const Pixel& color )
{
    backgroundColor_ = color;
    return *this;
}

const Font& Format::font() const
{
    return *font_;
}

const Pixel& Format::textColor() const
{
    return textColor_;
}

const Pixel& Format::backgroundColor() const
{
    return backgroundColor_;
}

}  // namespace lcd
