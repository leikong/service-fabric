#!/bin/bash

OutputFile="make.out"

make 2>&1 | tee ${OutputFile}

if [ $? -ne 0 ]; then
    echo "make failed - see errors in ${OutputFile}"
    exit 1
fi
