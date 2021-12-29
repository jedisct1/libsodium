#!/bin/sh
export TARGET_ARCH=x86_64
export CFLAGS="-Os -march=westmere"
NDK_PLATFORM=android-21 ARCH=x86_64 HOST_COMPILER=x86_64-linux-android "$(dirname "$0")/android-build.sh"
