#!/bin/sh
export TARGET_ARCH=mips32
export CFLAGS="-Os -march=${TARGET_ARCH}"
ARCH=mips HOST_COMPILER=mipsel-linux-android "$(dirname "$0")/android-build.sh"
