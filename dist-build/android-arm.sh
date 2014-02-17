#!/bin/sh
export CFLAGS="-Os -mthumb"
export LDFLAGS="-mthumb"
TARGET_ARCH=arm HOST_COMPILER=arm-linux-androideabi "$(dirname "$0")/android-build.sh"
