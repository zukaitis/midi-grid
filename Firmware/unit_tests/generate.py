#!/usr/bin/python3

from ninja import ninja_syntax
import os
import json
import argparse

def remove_file_extension( filename: str ) -> str:
    (root, _extension) = os.path.splitext( filename )
    return root

def remove_variable_prefix( filename: str ) -> str:
    if '$' == filename[0]:
        return filename[filename.index( os.path.sep ):]
    return filename

def generate_single_build_file( test_path: str ):
    unit_test_root_dir = os.path.dirname( os.path.realpath( __file__ ) ) + os.path.sep

    if os.path.sep != test_path[-1]:
        test_path += os.path.sep
    if not os.path.isabs( test_path ):
        test_path = os.getcwd() + os.path.sep + test_path

    common_settings_filename = 'settings.json'
    common_settings_full_filename = unit_test_root_dir + common_settings_filename
    try:
        with open( common_settings_full_filename, 'r' ) as read_file:
            common_settings = json.load( read_file )
    except FileNotFoundError:
        print( common_settings_filename + ' file does not exist, generation failed')
        return

    settings_filename = 'build_settings.json'
    settings_full_filename = test_path + settings_filename
    try:
        with open( settings_full_filename, 'r' ) as read_file:
            settings = json.load( read_file )
    except FileNotFoundError:
        print( settings_full_filename + ' file does not exist, generation failed')
        return

    build_filename = 'build.ninja'
    build_full_filename = test_path + build_filename
    ninja_writer = ninja_syntax.Writer( open( build_full_filename, 'w' ) )
    n = ninja_writer

    script_full_filename = __file__
    script_filename = os.path.basename( script_full_filename )

    n.comment( 'This file is generated using ' + script_filename + ' script, do not edit it by hand!' )
    n.comment( 'Settings are parsed from ' + settings_filename + ' and ' + common_settings_filename + ' files' )
    n.newline()

    n.variable( 'ninja_required_version', '1.8' )
    n.newline()

    target_filename = os.path.basename( os.path.dirname( test_path ) )
    target_full_filename = test_path + target_filename
    if 'target' in settings:
        if os.path.isabs( settings['target'] ):
            target_full_filename = settings['target']
        else:
            target_full_filename = test_path + settings['target']
        target_filename = os.path.basename( target_full_filename )

    n.comment( 'Output target' )
    n.variable( 'target', target_full_filename )
    n.variable( 'target_path', test_path )
    n.newline()

    n.comment( 'Directory variables' )
    for key, value in common_settings['directories'].items():
        if '.' == value:
            value = unit_test_root_dir
        if not os.path.isabs( value ):
            value = unit_test_root_dir + value
        value = os.path.normpath( value )
        value += os.path.sep
        n.variable( key, value )
    n.newline()

    n.comment( 'Common variables')
    for key, value in common_settings['ninja_variables'].items():
        n.variable( key, value )
    n.newline()

    inc_flags = list( '-I' + i for i in common_settings['include_directories'] )
    n.variable( 'inc_flags', inc_flags )
    n.newline()

    n.comment( 'Build and link rules' )
    n.rule( 'cxx',
        command='${compiler} -MMD -MT ${out} -MF ${out}.d ${compiler_flags} ${inc_flags} -c ${in} -o ${out}',
        depfile='${out}.d',
        description='CXX ${out}' )
    n.newline()

    n.rule( 'link',
        command='${linker} -o ${out} ${in} ${linker_flags}',
        description='LINK ${out}' )
    n.newline()

    n.comment( 'Regenerate build files if build script changes.' )
    n.rule( 'configure',
        command='python3 ' + script_full_filename + ' ${target_path}',
        generator=True,
        description='Generating ${out} file' )
    n.newline()

    n.rule( 'run',
        command='$in --gtest_color=yes | tee $out',
        description='Running the test' )
    n.newline()

    n.comment( 'Build instructions' )
    objs = list()
    for f in settings['source_files']:
        o = '${object_dir}' + remove_file_extension( remove_variable_prefix( f ) ) + '.o'
        n.build( o, 'cxx', f )
        objs.append( o )
    n.newline()

    n.comment( 'Target linker instruction' )
    n.build( '${target}', 'link', objs )
    n.newline()

    n.comment( 'Build file generation instruction' )
    n.build( build_filename, 'configure', implicit=[script_full_filename, settings_filename, common_settings_full_filename] )
    n.newline()

    n.comment( 'Test run instruction' )
    n.build( '${target_path}/result.txt', 'run', '${target}' )
    n.build( 'run', 'phony ${target_path}/result.txt' )
    n.newline()

    n.build( 'all', 'phony ' + build_filename + ' ${target}' )
    n.default( 'all' )

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument( 'path', type = str, nargs = '?', default = None )
    arguments = parser.parse_args()

    if arguments.path != None:
        generate_single_build_file( arguments.path )
    else:
        tests = next( os.walk( os.path.dirname( os.path.realpath( __file__ ) ) + os.path.sep + 'tests' ) )[1]

        for t in tests:
            generate_single_build_file( os.path.dirname( os.path.abspath( __file__ ) ) + os.path.sep + 'tests' + os.path.sep + t )
