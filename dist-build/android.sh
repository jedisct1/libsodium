#! /bin/sh

if [ -z "$NDK_ROOT" ]; then
    echo "You should probably set NDK_ROOT to the directory containing"
    echo "the Android NDK"
fi

if [ ! -f ./configure ]; then
	echo "Can't find ./configure. Wrong directory or haven't run autogen.sh?"
	exit 1
fi

export NDK_PLATFORM=${NDK_PLATFORM:-android-14}
export NDK_ROOT=${NDK_ROOT:-/usr/local/Cellar/android-ndk/9}
export TARGET_ARCH=arm
export TARGET="${TARGET_ARCH}-linux-androideabi"
export MAKE_TOOLCHAIN="${NDK_ROOT}/build/tools/make-standalone-toolchain.sh"

export PREFIX="$(pwd)/libsodium-android"
export TOOLCHAIN_DIR="$(pwd)/android-toolchain"
export PATH="${PATH}:${TOOLCHAIN_DIR}"
export CFLAGS="-Os -mthumb"
export LDFLAGS="-mthumb"

$MAKE_TOOLCHAIN --platform="$NDK_PLATFORM" --arch="$TARGET_ARCH" \
                --install-dir="$TOOLCHAIN_DIR"

./configure --host=arm-linux-androideabi \
            --with-sysroot="${TOOLCHAIN_DIR}/sysroot" \
            --disable-pie \
            --disable-shared \
            --prefix="$PREFIX" && \
make clean && \
make -j3 install && \
echo "libsodium has been installed into $PREFIX"
