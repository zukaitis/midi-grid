// This file is generated using convert.py script
// Editing it by hand would not be the best idea if you value your time

#include "lcd/ImageMono.h"

namespace lcd
{
namespace image
{

static const etl::array<uint8_t, 27> data = {
    0x80, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xFE, 0xFF, 0x7F, 0xFF, 0xFF, 0x7F, 0xFE, 0xFF, 0x7F, 0xF8,
    0x00, 0x00, 0xE0, 0x00, 0x00, 0x80, 0x00, 0x00
};

static const ImageMono img( ImageMono::DataView( data ), 9, 23 );

const ImageMono& up_arrow_9x23 = img;

}  // namespace image
}  // namespace lcd