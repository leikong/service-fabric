#!/bin/bash

OutputFile="ninja.out"

ninja 2>&1 | tee ${OutputFile}

if (( $? != 0 )); then
    echo "ninja failed - see errors in ${OutputFile}"
    exit 1
fi
