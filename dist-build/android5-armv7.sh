#!/bin/sh
export CFLAGS="-Os -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -marm -march=armv7-a -fvisibility=default -fPIE"
export LDFLAGS="-rdynamic -fPIE -pie"
TARGET_ARCH=arm HOST_COMPILER=arm-linux-androideabi "$(dirname "$0")/android-build.sh"
