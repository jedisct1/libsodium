#! /bin/sh

if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "You should probably set ANDROID_NDK_HOME to the directory containing"
    echo "the Android NDK"
    exit
fi

if [ ! -f ./configure ]; then
	echo "Can't find ./configure. Wrong directory or haven't run autogen.sh?"
	exit 1
fi

if [ "x$TARGET_ARCH" = 'x' ] || [ "x$ARCH" = 'x' ] || [ "x$HOST_COMPILER" = 'x' ]; then
    echo "You shouldn't use android-build.sh directly, use android-[arch].sh instead"
    exit 1
fi

export MAKE_TOOLCHAIN="${ANDROID_NDK_HOME}/build/tools/make-standalone-toolchain.sh"

export PREFIX="$(pwd)/libsodium-android-${TARGET_ARCH}"
export TOOLCHAIN_DIR="$(pwd)/android-toolchain-${TARGET_ARCH}"
export PATH="${PATH}:${TOOLCHAIN_DIR}/bin"

rm -rf "${TOOLCHAIN_DIR}" "${PREFIX}"

bash $MAKE_TOOLCHAIN --platform="${NDK_PLATFORM:-android-16}" \
    --arch="$ARCH" --install-dir="$TOOLCHAIN_DIR" && \
./configure \
    --disable-soname-versions \
    --enable-minimal \
    --host="${HOST_COMPILER}" \
    --prefix="${PREFIX}" \
    --with-sysroot="${TOOLCHAIN_DIR}/sysroot" && \
make clean && \
make -j3 install && \
echo "libsodium has been installed into ${PREFIX}"
