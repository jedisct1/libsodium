#! /bin/sh

if [ -z "$NDK_PLATFORM" ]; then
  export NDK_PLATFORM="android-24"
  export NDK_PLATFORM_COMPAT="${NDK_PLATFORM_COMPAT:-android-16}"
else
  export NDK_PLATFORM_COMPAT="${NDK_PLATFORM_COMPAT:-${NDK_PLATFORM}}"
fi

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

echo
echo "Building for platform [${NDK_PLATFORM}], retaining compatibility with platform [${NDK_PLATFORM_COMPAT}]"
echo

bash $MAKE_TOOLCHAIN --platform="$NDK_PLATFORM_COMPAT" \
    --arch="$ARCH" --install-dir="$TOOLCHAIN_DIR" || exit 1

./configure \
    --disable-soname-versions \
    --enable-minimal \
    --host="${HOST_COMPILER}" \
    --prefix="${PREFIX}" \
    --with-sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

if [ "$NDK_PLATFORM" != "$NDK_PLATFORM_COMPAT" ]; then
  egrep '^#define ' config.log | sort -u > config-def-compat.log
  echo
  echo "Configuring again for platform [${NDK_PLATFORM}]"
  echo
  bash $MAKE_TOOLCHAIN --platform="$NDK_PLATFORM" \
      --arch="$ARCH" --install-dir="$TOOLCHAIN_DIR" || exit 1

  ./configure \
      --disable-soname-versions \
      --enable-minimal \
      --host="${HOST_COMPILER}" \
      --prefix="${PREFIX}" \
      --with-sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

  egrep '^#define ' config.log | sort -u > config-def.log
  if ! cmp config-def.log config-def-compat.log; then
    echo "Platform [${NDK_PLATFORM}] is not backwards-compatible with [${NDK_PLATFORM_COMPAT}]" >&2
    diff -u config-def.log config-def-compat.log >&2
    exit 1
  fi
  rm -f config-def.log config-def-compat.log
fi

make clean && \
make -j3 install && \
echo "libsodium has been installed into ${PREFIX}"
