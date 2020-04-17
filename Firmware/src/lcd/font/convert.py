#!/usr/bin/python3

import argparse
import os
import bdflib.reader
import math
import textwrap

first_symbol = ord(' ')
last_symbol = ord('~') + 1

def get_max_height( font : bdflib.model.Font ):
    max_height = 0
    for symbol in range( first_symbol, last_symbol ):
        glyph = font[symbol]
        if glyph.bbY + glyph.bbH > max_height:
            max_height = glyph.bbY + glyph.bbH
    return max_height

def get_negative_offset( font : bdflib.model.Font ):
    negative_offset = 0
    for symbol in range( first_symbol, last_symbol ):
        glyph = font[symbol]
        if glyph.bbY < negative_offset:
            negative_offset = glyph.bbY
    return negative_offset

def convert( file_path : str ):
    font = bdflib.reader.read_bdf( open( file_path, 'rb' ) )

    height = get_max_height( font )
    negative_offset = get_negative_offset( font )
    bytes_per_column = math.ceil((height - negative_offset) / 8)

    font_array = []
    font_map = [0]

    for symbol in range( first_symbol, last_symbol ):
        glyph = font[symbol]
        glyph_array = [0] * glyph.bbW * bytes_per_column
        if ord(' ') == symbol:  # special treatment for the space character
            glyph_array = [0] * glyph.advance * bytes_per_column
        else:
            for row in reversed( range( len( glyph.data ) ) ):  # going top to bottom
                y = height - 1 - glyph.bbY - row
                for x in range( glyph.bbW ):
                    bit_set = (0 != ((glyph.data[row] >> x) & 1))
                    glyph_array[x * bytes_per_column + y // 8] |= bit_set << (y % 8)
        glyph_array.reverse()  # what a dumb format this bdf is!
        font_array += glyph_array
        font_map.append( font_map[-1] + len( glyph_array ) )

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument( 'path', type = str, nargs = '?', default = None )
    arguments = parser.parse_args()

    arguments.path = 'submodules/u8g2/tools/font/ttf/nokiafc22.bdf'

    if arguments.path != None:
        convert( arguments.path )

