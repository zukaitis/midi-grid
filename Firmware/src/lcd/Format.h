#pragma once

#include "types/Color.h"
#include <etl/array.h>
#include <etl/array_view.h>

namespace lcd
{

class Font;

namespace font
{
extern const Font& nokia_8p;
extern const Font& monoton_80p;
extern const Font& sarpanch_79p;
extern const Font& rubik_16p;
extern const Font& rubik_24p;
}

using PixelView = etl::array_view<const Color>;

enum class Justification
{
    LEFT,
    RIGHT,
    CENTER
};

class Format
{
public:
    Format();

    Format& font( const Font& font );
    Format& textColor( const Color& color );
    Format& backgroundColor( const Color& color );
    Format& justification( Justification justification );

    const Font& font() const;
    const Color& textColor() const;
    const Color& backgroundColor() const;
    bool isBackgroundColorSet() const;
    Justification justification() const;

private:
    const Font* font_;
    Color textColor_;
    Color backgroundColor_;
    bool backgroundColorSet_;
    Justification justification_;
};

}  // namespace lcd
