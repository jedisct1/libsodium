#!/bin/sh
export TARGET_ARCH=armv8-a
export CFLAGS="-Os -march=${TARGET_ARCH}"
ARCH=arm64 HOST_COMPILER=aarch64-linux-android "$(dirname "$0")/android-build.sh"
