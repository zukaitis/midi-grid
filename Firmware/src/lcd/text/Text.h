#include "lcd/text/TextInterface.h"

namespace lcd
{

class ImageInterface;

class Text: public TextInterface
{
public:
    explicit Text( ImageInterface* image );
    ~Text() override;

    uint16_t print( const etl::string_view& string, const Coordinates& coords, const Format& format ) override;
    uint16_t print( const etl::string_view& string, uint16_t y, const Format& format ) override;

private:
    uint16_t putString( const etl::string_view& string, const Coordinates& coords, const Format& format );
    uint16_t putChar( char c, const Coordinates& coords, const lcd::Font& font, uint8_t textColorIndex, uint8_t backgroundColorIndex = 0 );
    uint16_t putCharacterSpace( const Coordinates& coords, const lcd::Font& font, uint8_t backgroundColorIndex = 0 );

    uint16_t calculateX( Justification justification );
    static Coordinates calculateCoordinates( const etl::string_view& string, const Coordinates& coords, const Format& format );

    ImageInterface& image_;
};

}  // namespace lcd
