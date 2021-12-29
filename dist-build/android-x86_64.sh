#!/bin/sh
export CFLAGS="-Os -march=westmere"
NDK_PLATFORM=android-21 HOST_COMPILER=x86_64-linux-android TARGET_ARCH_DIR="${HOST_COMPILER}" "$(dirname "$0")/android-build.sh"
