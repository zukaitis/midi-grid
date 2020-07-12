#pragma once

#include "lcd/text/Format.h"
#include "types/Coordinates.h"
#include <etl/string_view.h>

namespace lcd
{

class TextInterface
{
public:
    virtual ~TextInterface() = default;

    virtual uint16_t print( const etl::string_view& string, const Coordinates& coords, const Format& format ) { return 0; };
    virtual uint16_t print( const etl::string_view& string, uint16_t y, const Format& format ) { return 0; };
};

}  // namespace lcd
