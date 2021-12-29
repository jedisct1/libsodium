#!/bin/sh
export CFLAGS="-Os -march=i686"
HOST_COMPILER=i686-linux-android TARGET_ARCH_DIR="${HOST_COMPILER}" "$(dirname "$0")/android-build.sh"
