#! /bin/sh

# Note that this script requires droid-wrapper
if [ x`which droid-gcc` = x ]; then
	echo "This build script requires droid-wrapper."
	echo https://github.com/tmurakam/droid-wrapper
	exit 1
fi

if [ -z "$NDK_ROOT" ]; then
    echo "You should probably set NDK_ROOT to the directory containing"
    echo "the Android NDK"
fi

if [ ! -f ./configure ]; then
	echo "Can't find ./configure.  Wrong directory or haven't run autogen.sh?"
	exit 1
fi

export CFLAGS="-Os -mthumb"
export DROID_HOST=darwin-x86
if uname -a | grep -q -i linux; then
	export DROID_HOST=linux-x86
fi
export LDFLAGS="-mthumb"
export NDK_PLATFORM=9
export NDK_ROOT=${NDK_ROOT:-/usr/local/Cellar/android-ndk/9}
export NDK_ANDROID_SOURCES="${NDK_ROOT}/sources/android"
export TARGET_TOOLCHAIN_VERSION=4.6
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
export PREFIX="$(pwd)/libsodium-android"

./configure --host=arm-linux-androideabi \
            --disable-pie \
            --disable-shared \
            --prefix="$PREFIX" && \
make clean && \
make -j3 install && \
echo "libsodium has been installed into $PREFIX"
