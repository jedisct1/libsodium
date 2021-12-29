#!/bin/sh
export TARGET_ARCH=arm64-v8a
export CFLAGS="-Os -march=armv8-a"
NDK_PLATFORM=android-21 ARCH=arm64 HOST_COMPILER=aarch64-linux-android "$(dirname "$0")/android-build.sh"
