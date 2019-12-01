#!/usr/bin/python3

from ninja import ninja_syntax
import os
import json

def remove_file_extension( filename: str ) -> str:
    return os.path.splitext( filename )[0]

BUILD_FILENAME = 'build.ninja'
ninja_writer = ninja_syntax.Writer( open( BUILD_FILENAME, 'w' ) )
n = ninja_writer

SETTINGS_FILENAME = 'build_settings.json'
with open( SETTINGS_FILENAME, 'r') as read_file:
    settings = json.load( read_file )

SCRIPT_NAME = os.path.basename( __file__ )

n.comment( 'This file is generated using ' + SCRIPT_NAME + ' script, do not edit it by hand!' )
n.comment( 'Settings are parsed from ' + SETTINGS_FILENAME + ' file' )
n.newline()

n.variable( 'ninja_required_version', '1.8' )
n.newline()

out_dir = ''
if 'output_directory' in settings:
    out_dir = settings['output_directory']
    if out_dir[-1] != '/' or '\\':
        out_dir += '/'
n.variable( 'out_dir', out_dir )
n.newline()

n.variable( 'target', settings['target'] )
n.newline()

n.variable( 'cc', settings['c_compiler'] )
n.variable( 'cxx', settings['cxx_compiler'] )
n.variable( 'as', settings['asm_compiler'] )
n.variable( 'ld', settings['linker'] )
n.variable( 'objcopy', settings['object_copy_utility'] )
n.variable( 'objdump', settings['object_information_utility'] )
n.variable( 'size', settings['output_size_report_utility'] )
n.newline()

n.variable( 'arch_flags', settings['arch_flags'] )
n.variable( 'c_flags', settings['c_flags'] )
n.variable( 'cxx_flags', settings['cxx_flags'] )

inc_flags = list( '-I' + i for i in settings['include_directories'] )
n.variable( 'inc_flags', inc_flags )

ld_flags = settings['linker_flags']
ld_flags.append( '-T ' + settings['linker_script'] )
n.variable( 'ld_flags', ld_flags )
n.newline()

n.rule( 'as',
    command='$as -MMD -MT $out -MF $out.d $arch_flags $c_flags -c $in -o $out',
    depfile='$out.d',
    # deps='gcc',
    description='AS $out' )
n.newline()

n.rule( 'cc',
    command='$cc -MMD -MT $out -MF $out.d $arch_flags $c_flags $inc_flags -c $in -o $out',
    depfile='$out.d',
    # deps='gcc',
    description='CC $out' )
n.newline()

n.rule( 'cxx',
    command='$cxx -MMD -MT $out -MF $out.d $arch_flags $c_flags $cxx_flags $inc_flags -c $in -o $out',
    depfile='$out.d',
    # deps='gcc',
    description='CXX $out' )
n.newline()

n.rule( 'link',
    command='$ld $arch_flags $ld_flags -o $out $in $libs',
    description='LINK $out' )
n.newline()

n.rule( 'size',
    command='$size $in',
    description='Size report' )
n.newline()

n.rule( 'bin',
    command='$objcopy -O binary $in $out',
    description='BIN $out' )
n.newline()

n.rule( 'hex',
    command='$objcopy -O ihex $in $out',
    description='HEX $out' )
n.newline()

n.comment( 'Regenerate build files if build script changes.' )
n.rule( 'configure',
    command='python3 ' + SCRIPT_NAME,
    generator=True,
    description='Generating $out file' )
n.newline()

objs = list()

for f in settings['asm_source_files']:
    o = out_dir + remove_file_extension( f ) + '.o'
    n.build( o, 'as', f )
    objs.append( o )
n.newline()

for f in settings['c_source_files']:
    o = out_dir + remove_file_extension( f ) + '.o'
    n.build( o, 'cc', f )
    objs.append( o )
n.newline()

for f in settings['cxx_source_files']:
    o = out_dir + remove_file_extension( f ) + '.o'
    n.build( o, 'cxx', f )
    objs.append( o )
n.newline()

n.build( '$target', 'link', objs )
n.build( 'size_report', 'size', '$target' )
n.build( 'output/trrash.bin', 'bin', '$target' )
n.build( 'Debug/midi-grid.hex', 'hex', '$target' )
n.build( BUILD_FILENAME, 'configure', implicit=[SCRIPT_NAME, SETTINGS_FILENAME] )

n.build( 'all', 'phony ' + BUILD_FILENAME + ' $target size_report Debug/midi-grid.hex' )
n.default( 'all' )
