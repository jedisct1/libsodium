#!/bin/sh
export CFLAGS="-Os -march=armv8-a"
NDK_PLATFORM=android-21 ARCH=arm64 HOST_COMPILER=aarch64-linux-android TARGET_ARCH_DIR="${HOST_COMPILER}" "$(dirname "$0")/android-build.sh"
