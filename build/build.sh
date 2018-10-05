#!/bin/bash

SourceDirectory="/media/alexwun/Data_Ext4/Horizon/service-fabric/src/prod"
ClangVersion=6.0
CMakeGenerator="-G Ninja"
OutputFile="cmake.out"

CC=/usr/lib/llvm-${ClangVersion}/bin/clang
CXX=/usr/lib/llvm-${ClangVersion}/bin/clang++

cmake $CMakeGenerator \
    -DCMAKE_C_COMPILER=$CC \
    -DCMAKE_CXX_COMPILER=$CXX \
    $SourceDirectory 2>&1 | tee $OutputFile

if (( $? -ne 0 )); then
    echo "cmake failed - see errors in ${OutputFile}"
    exit 1
fi

if [[ $CMakeGenerator == "" ]]; then
    ./make.sh
else
    ./ninja.sh
fi
