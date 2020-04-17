#include "lcd/font/Font.h"

namespace lcd
{

Font::Font( uint8_t numberOfCharacters, uint8_t firstCharacterOffset, DataView data, MapView map,
        uint8_t bytesPerColumn, uint8_t gapWidth ):
    numberOfCharacters_( numberOfCharacters ),
    firstCharacterOffset_( firstCharacterOffset ),
    data_( data ),
    map_( map ),
    bytesPerColumn_( bytesPerColumn ),
    gapWidth_( gapWidth )
{
}

}  // namespace lcd
