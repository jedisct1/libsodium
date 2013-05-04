#! /bin/sh

export CFLAGS="-Os -mthumb"
export DROID_HOST=darwin-x86
export LDFLAGS="-mthumb"
export NDK_PLATFORM=9
export NDK_ROOT=/usr/local/Cellar/android-ndk/r8e
export NDK_ANDROID_SOURCES="${NDK_ROOT}/sources/android"
export TARGET_TOOLCHAIN_VERSION=4.4.3
export TARGET=arm-linux-androideabi
export NDK_TARGET="arm-linux-androideabi-${TARGET_TOOLCHAIN_VERSION}"
export AR=droid-ar
export AS=droid-as
export CC=droid-gcc
export LD=droid-ld
export NM=droid-nm
export OBJCOPY=droid-objcopy
export RANLIB=droid-ranlib
export STRIP=droid-strip

./configure --host=arm-linux-androideabi && make -j3
