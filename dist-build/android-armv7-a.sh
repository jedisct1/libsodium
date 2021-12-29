#!/bin/sh
export TARGET_ARCH=armeabi-v7a
export CFLAGS="-Os -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -marm -march=armv7-a"
ARCH=arm HOST_COMPILER=armv7a-linux-androideabi "$(dirname "$0")/android-build.sh"

