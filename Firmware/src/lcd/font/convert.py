#!/usr/bin/python3

import argparse
import os
import bdflib.reader
import math
import textwrap

output_file_header = '// This file is generated using ' + os.path.basename( __file__ ) + ' script' + '''
// Editing it by hand would not be the best idea if you value your time

#include "lcd/text/Font.h"

namespace lcd
{
namespace font
{

'''

output_file_footer = '''

}  // namespace font
}  // namespace lcd
'''

class Font:
    def __init__(self):
        self.first_symbol_ = ord(' ')
        self.last_symbol_ = ord('~') + 1

    def setNumbersRange(self):
        self.first_symbol_ = ord('+')
        self.last_symbol_ = ord('9') + 1

    def get_max_height(self, font : bdflib.model.Font):
        max_height = 0
        for symbol in range( self.first_symbol_, self.last_symbol_ ):
            glyph = font[symbol]
            if glyph.bbY + glyph.bbH > max_height:
                max_height = glyph.bbY + glyph.bbH
        return max_height

    def get_negative_offset(self, font : bdflib.model.Font):
        negative_offset = 0
        for symbol in range( self.first_symbol_, self.last_symbol_ ):
            glyph = font[symbol]
            if glyph.bbY < negative_offset:
                negative_offset = glyph.bbY
        return negative_offset

    def convert(self, input_file : str, output_file : str):
        font = bdflib.reader.read_bdf( open( input_file, 'rb' ) )

        height = self.get_max_height( font )
        negative_offset = self.get_negative_offset( font )
        total_height = height - negative_offset
        baseline_position = height - 1
        bytes_per_column = math.ceil(total_height / 8)

        data_array = []
        font_map = [0]

        for symbol in range( self.first_symbol_, self.last_symbol_ ):
            glyph = font[symbol]
            glyph_array = [0] * glyph.bbW * bytes_per_column
            if ord(' ') == symbol:  # special treatment for the space character
                glyph_array = [0] * glyph.advance * bytes_per_column
            else:
                for row in reversed( range( len( glyph.data ) ) ):  # going top to bottom
                    y = height - 1 - glyph.bbY - row
                    for x in range( glyph.bbW ):
                        if (0 != ((glyph.data[row] >> x) & 1)):
                            glyph_array[(glyph.bbW-1-x) * bytes_per_column + y // 8] |= 1 << (y % 8)
            # what a dumb format this bdf is!
            data_array += glyph_array
            font_map.append( font_map[-1] + len( glyph_array ) )

        number_of_characters = self.last_symbol_ - self.first_symbol_
        gap_width = font[ord('0')].advance - font[ord('0')].bbW

        output = open( output_file,'w' ) 
        output.write( output_file_header )

        wrapper = textwrap.TextWrapper( width = 120, initial_indent = '    ', subsequent_indent = '    ' )

        output.write( 'static const etl::array<uint8_t, ' + str(len( data_array )) + '> data = {\n' )
        for line in wrapper.wrap( ', '.join( '0x%02X'%i for i in data_array ) ):
            output.write( line + '\n' )
        output.write( '};\n\n' )

        output.write( 'static const etl::array<uint16_t, ' + str(len( font_map )) + '> map = {\n' )
        for line in wrapper.wrap( ', '.join( str(i) for i in font_map ) ):
            output.write( line + '\n' )
        output.write( '};\n\n' )

        font_name = os.path.splitext( os.path.basename( output_file ) )[0]
        output.write( 'static const Font fnt( ' + \
            str(number_of_characters) + ', ' + str(self.first_symbol_) + \
            ', Font::GlyphView( data ), Font::MapView( map ), ' + \
            str(total_height) + ', ' + str(baseline_position) + ', ' + str(gap_width) + ' );\n\n' )
        output.write( 'const Font& ' + str(font_name) + ' = fnt;' )

        output.write( output_file_footer )
        output.close() 

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument( 'input', type = str, nargs = '?', default = None )
    parser.add_argument( 'output', type = str, nargs = '?', default = None )
    parser.add_argument("-n", "--numbers", action="store_true",
        help="only convert +,-./0123456789 characters")
    arguments = parser.parse_args()

    font = Font()
    if (arguments.numbers):
        font.setNumbersRange()

    if (arguments.input != None) and (arguments.output != None):
        font.convert(arguments.input, arguments.output)

