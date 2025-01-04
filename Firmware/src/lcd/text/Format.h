#pragma once

#include "etl/string_view.h"
#include "lcd/text/Font.h"
#include "types/Color.h"
#include <etl/array.h>
#include <etl/array_view.h>
#include <etl/cstring.h>

namespace lcd
{

namespace font
{
extern const Font& nokia_8p;
extern const Font& monoton_80p;
extern const Font& sarpanch_79p;
extern const Font& rubik_16p;
extern const Font& rubik_24p;
}

enum class Justification : uint8_t
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
    Format& maxWidth( uint16_t width );
    Format& abbreviationSuffix( const etl::string_view& suffix );

    const Font& font() const;
    const Color& textColor() const;
    const Color& backgroundColor() const;
    bool isBackgroundColorSet() const;
    Justification justification() const;
    uint16_t maxWidth() const;
    etl::string_view abbreviationSuffix() const;

    static const uint16_t unlimitedWidth = 0xFFFF;

private:
    const Font* font_;
    Color textColor_;
    Color backgroundColor_;
    bool backgroundColorSet_;
    Justification justification_;
    uint16_t maxWidth_;

    using AbbreviationSuffix = etl::string<3>;
    AbbreviationSuffix abbreviationSuffix_;
};

}  // namespace lcd
