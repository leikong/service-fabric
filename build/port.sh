#!/bin/bash

ServiceFabricRoot="/media/alexwun/Data_Ext4/ServiceFabric"
HorizonRoot="/media/alexwun/Data_Ext4/Horizon/service-fabric"
NamePattern=""
PathPattern=""
DoCopy=false

while (( $# > 0 )); do
    if [[ $1 == "-n" ]]; then
        shift
        NamePattern=$1
    elif [[ $1 == "-p" ]]; then
        shift
        PathPattern=$1
    elif [[ $1 == "-c" ]]; then
        DoCopy=true
    fi

    shift
done

if [[ $NamePattern == "" ]]; then
    echo "Usage:   ./port.sh -n <name pattern> [-p <path pattern> [-c (copy)]]"
    echo "Example: ./port.sh -n Claim.h"
    echo "Example: ./port.sh -n Claim.h -p *ServiceModel/C*"
    exit 1
fi

Arguments="-name ${NamePattern}"

if [[ $PathPattern != "" ]]; then
    Arguments="${Arguments} -path ${PathPattern}"
fi

if [[ $DoCopy == true ]]; then
    Arguments="${Arguments} -exec cp --parents {} ${HorizonRoot} ;"
    echo "cd ${ServiceFabricRoot} && find . ${Arguments}"
    cd $ServiceFabricRoot && find . $Arguments
else
    find $ServiceFabricRoot $Arguments
fi
