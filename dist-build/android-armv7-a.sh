#!/bin/sh
export CFLAGS="-Os -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -marm -march=armv7a"
HOST_COMPILER=armv7a-linux-androideabi TARGET_ARCH_DIR=arm-linux-androideabi "$(dirname "$0")/android-build.sh"

