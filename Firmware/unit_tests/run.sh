#!/bin/bash

print_usage() {
    printf "Usage:
  -m flag specifies, that only tests, affected by recent changes shall be ran
  -n flag specifies, that if build.ninja file is not found in test_path directory, the script shouldn't regenerate it"
}

WORKING_DIR=$pwd
UNIT_TESTS_DIR=$(dirname $(readlink -f $0))

BUILD_FILENAME=build.ninja

modified_flag=false
no_generation_flag=false

while getopts 'mn' flag; do
    case "${flag}" in
        m) modified_flag=true ;;
        n) no_generation_flag=true ;;
        *) print_usage
            exit 1 ;;
    esac
done

for test_path in ${UNIT_TESTS_DIR}/tests/*/
do
    test_name=${test_path%*/} # remove the trailing "/"
    test_name=${test_name##*/} # print everything after the final "/"

    if [ -f ${test_path}${BUILD_FILENAME} ]; then
        cd ${test_path}
        if [ "${modified_flag}" = true ]; then
            ninja run
        else
            ninja all
            ./${test_name}
        fi
    else
        if [ "${no_generation_flag}" = false ]; then
            ${UNIT_TESTS_DIR}/generate.py ${test_path}
            if [ -f ${test_path}${BUILD_FILENAME} ]; then
                cd ${test_path}
                if [ "${modified_flag}" = true ]; then
                    ninja run
                else
                    ninja all
                    ./${test_name}
                fi
            else
                printf "${BUILD_FILENAME} file for ${test_name} folder was not generated, use -n flag to ignore it\n"
            fi
        fi
    fi
done
cd ${WORKING_DIR}
