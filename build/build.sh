#!/bin/bash

SourceDirectory="../src/prod"
ClangVersion=6.0
CMakeGenerator="-G Ninja"
OutputFile="cmake.out"
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

CC=/usr/lib/llvm-${ClangVersion}/bin/clang
CXX=/usr/lib/llvm-${ClangVersion}/bin/clang++

cmake $CMakeGenerator \
    -DCMAKE_C_COMPILER=$CC \
    -DCMAKE_CXX_COMPILER=$CXX \
    $SourceDirectory 2>&1 | tee $OutputFile

if [[ $? -ne 0 ]]; then
    echo "cmake failed - see errors in ${OutputFile}"
    exit 1
fi

if [[ $CMakeGenerator == "" ]]; then
    $CUR_DIR/make.sh
else
    $CUR_DIR/ninja.sh
fi
