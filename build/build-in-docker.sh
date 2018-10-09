#!/bin/bash -x

## Intialize variables
CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_PARAMS=""
TARGET_OS="ubuntu"
REPO_ROOT=$CUR_DIR/../

print_help()
{
	 cat <<EOF
Usage:
runbuild.sh [build arguments] [-h] [-centos]
    -h, --help: Displays this help text
    -centos: Run the build against CentOS instead of the default Ubuntu 15.04

This script starts a container then calls src/build.sh within it. It
accepts all build.sh arguments except -d. See below for the arguments
to build.sh.
EOF
}

#Read parameters
while (( "$#" )); do
    if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
	print_help
        $CUR_DIR/src/build.sh -h
        exit -1
    elif [ "$1" == "-centos" ]; then
        TARGET_OS="centos"
    else
        BUILD_PARAMS="$BUILD_PARAMS $1"
    fi
    shift
done

IMAGE_VERSION=`cat ${CUR_DIR}/${TARGET_OS}_image_version`
FULL_IMAGE_NAME=microsoft/horizon-build-${TARGET_OS}
OUT_DIR=$REPO_ROOT/out_${TARGET_OS}
BUILD_CMD="cd /out/ && /build/build.sh -all -d $BUILD_PARAMS"
DOCKER_FILE=$CUR_DIR/Dockerfile.$TARGET_OS

mkdir -p $OUT_DIR
docker build -t $FULL_IMAGE_NAME:latest -f $DOCKER_FILE $REPO_ROOT
docker tag $FULL_IMAGE_NAME:latest $FULL_IMAGE_NAME:$IMAGE_VERSION

if [ "$(docker images -q $FULL_IMAGE_NAME:$IMAGE_VERSION 2> /dev/null)" == "" ]; then
    echo "Docker either failed to build image $FULL_IMAGE_NAME:$IMAGE_VERSION  for target OS ${TARGET_OS} or OS is not currently supported. Exiting."
    exit 1
fi

docker run \
    --rm \
    --net=host \
    --cap-add=NET_ADMIN \
    -ti \
    -v "$OUT_DIR":/out \
    -v "$REPO_ROOT"/deps:/deps \
    -v "$REPO_ROOT"/external:/external \
    -v "$REPO_ROOT"/src:/src \
    -v "$REPO_ROOT"/.config:/.config \
    -v "$REPO_ROOT"/build:/build \
    $FULL_IMAGE_NAME:$IMAGE_VERSION \
    bash -c "$BUILD_CMD"

