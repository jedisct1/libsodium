#!/bin/sh
export TARGET_ARCH=i686
export CFLAGS="-Os -march=${TARGET_ARCH}"
export LDFLAGS="-Wl,-z,max-page-size=16384"
ARCH=x86 HOST_COMPILER=i686-linux-android "$(dirname "$0")/android-build.sh"
