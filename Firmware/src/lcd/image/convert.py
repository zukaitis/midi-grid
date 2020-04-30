#!/usr/bin/python3

import argparse
import sys
import os
from PIL import Image
import math
import textwrap

output_file_header = '// This file is generated using ' + os.path.basename( __file__ ) + ' script' + '''
// Editing it by hand would not be the best idea if you value your time

#include "lcd/Image.h"

namespace lcd
{
namespace image
{

'''

output_file_footer = '''

}  // namespace image
}  // namespace lcd
'''

def convert( input_file : str, output_file : str ):
    image = Image.open( input_file ).convert('1')
    bytes_per_column = math.ceil(image.height / 8)
    
    data_array = [0] * image.width * bytes_per_column
    for x in range( 0, image.width ):
        for y in range( 0, image.height ):
            if image.getpixel( (x, y) ) < 127:
                data_array[x * bytes_per_column + y // 8] |= 1 << (y % 8)

    output = open( output_file,'w' ) 
    output.write( output_file_header )

    wrapper = textwrap.TextWrapper( width = 120, initial_indent = '    ', subsequent_indent = '    ' )

    output.write( 'static const etl::array<uint8_t, ' + str(len( data_array )) + '> data = {\n' )
    for line in wrapper.wrap( ', '.join( '0x%02X'%i for i in data_array ) ):
        output.write( line + '\n' )
    output.write( '};\n\n' )

    image_name = os.path.splitext( os.path.basename( output_file ) )[0]
    output.write( 'static const Image img( Image::DataView( data ), ' + \
        str(image.width) + ', ' + str(image.height) + ' );\n\n' )
    output.write( 'const Image& ' + str(image_name) + ' = img;' )

    output.write( output_file_footer )
    output.close() 


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument( 'input', type = str, nargs = '?', default = None )
    parser.add_argument( 'output', type = str, nargs = '?', default = None )
    arguments = parser.parse_args()

    if (arguments.input == None):
        sys.exit( 1 )

    if (arguments.output == None):
        arguments.output = os.path.splitext( arguments.input )[0] + '.cpp'

    convert( arguments.input, arguments.output )
