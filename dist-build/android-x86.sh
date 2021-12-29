#!/bin/sh
export TARGET_ARCH=x86
export CFLAGS="-Os -march=i686"
ARCH=x86 HOST_COMPILER=i686-linux-android "$(dirname "$0")/android-build.sh"
