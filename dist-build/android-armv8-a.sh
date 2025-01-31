#!/bin/sh
export TARGET_ARCH=armv8-a+crypto
export CFLAGS="-Os -march=${TARGET_ARCH}"
export LDFLAGS="-Wl,-z,max-page-size=16384"
ARCH=arm64 HOST_COMPILER=aarch64-linux-android "$(dirname "$0")/android-build.sh"
