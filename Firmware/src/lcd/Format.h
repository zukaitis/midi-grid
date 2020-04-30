#pragma once

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

using Pixel = etl::array<uint8_t, 3>; // to be replaced with Color class
using PixelView = etl::array_view<const Pixel>;

namespace color
{
    const Pixel BLACK = {0, 0, 0};
    const Pixel RED = {255, 0, 0};
    const Pixel GREEN = {0, 255, 0};
    const Pixel BLUE = {0, 0, 255};
    const Pixel YELLOW = {255, 255, 0};
    const Pixel MAGENTA = {255, 0, 255};
    const Pixel CYAN = {0, 255, 255};
    const Pixel WHITE = {255, 255, 255};
    const Pixel ORANGE = {255, 125, 0};
    const Pixel SPRING_GREEN = {125, 255, 0};
    const Pixel TURQUOISE = {0, 255, 125};
    const Pixel OCEAN = {0, 125, 255};
    const Pixel VIOLET = {125, 0, 255};
    const Pixel RASPBERRY = {255, 0, 125};
}  // namespace color

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
    Format& textColor( const Pixel& color );
    Format& backgroundColor( const Pixel& color );
    Format& justification( Justification justification );

    const Font& font() const;
    const Pixel& textColor() const;
    const Pixel& backgroundColor() const;
    bool isBackgroundColorSet() const;
    Justification justification() const;

private:
    const Font* font_;
    Pixel textColor_;
    Pixel backgroundColor_;
    bool backgroundColorSet_;
    Justification justification_;
};

}  // namespace lcd
