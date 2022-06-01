#! /bin/sh

if [ -z "$NDK_PLATFORM" ]; then
  export NDK_PLATFORM="android-19"
fi
export NDK_PLATFORM_COMPAT="${NDK_PLATFORM_COMPAT:-${NDK_PLATFORM}}"
export NDK_API_VERSION="$(echo "$NDK_PLATFORM" | sed 's/^android-//')"
export NDK_API_VERSION_COMPAT="$(echo "$NDK_PLATFORM_COMPAT" | sed 's/^android-//')"

if [ -z "$ANDROID_NDK_HOME" ]; then
  echo "You should probably set ANDROID_NDK_HOME to the directory containing"
  echo "the Android NDK"
  exit 1
fi

if [ ! -f ./configure ]; then
  echo "Can't find ./configure. Wrong directory or haven't run autogen.sh?" >&2
  exit 1
fi

if [ -z "$TARGET_ARCH" ] || [ -z "$ARCH" ] || [ -z "$HOST_COMPILER" ]; then
  echo "You shouldn't use android-build.sh directly, use android-[arch].sh instead" >&2
  exit 1
fi

export PREFIX="$(pwd)/libsodium-android-${TARGET_ARCH}"
export TOOLCHAIN_OS_DIR="$(uname | tr '[:upper:]' '[:lower:]')-x86_64/"
export TOOLCHAIN_DIR="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/${TOOLCHAIN_OS_DIR}"
echo "$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/${TOOLCHAIN_OS_DIR}/${HOST_COMPILER}"

export PATH="${PATH}:${TOOLCHAIN_DIR}/bin"
SDK_VERSION_NUM=$(echo $NDK_PLATFORM | cut -d'-' -f2)
export CC=${CC:-"${HOST_COMPILER}${SDK_VERSION_NUM}-clang"}

echo
echo "Warnings related to headers being present but not usable are due to functions"
echo "that didn't exist in the specified minimum API version level."
echo "They can be safely ignored."
echo

echo
if [ "$NDK_PLATFORM" != "$NDK_PLATFORM_COMPAT" ]; then
  echo "Building for platform [${NDK_PLATFORM}], retaining compatibility with platform [${NDK_PLATFORM_COMPAT}]"
else
  echo "Building for platform [${NDK_PLATFORM}]"
fi
echo


if [ -z "$LIBSODIUM_FULL_BUILD" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
else
  export LIBSODIUM_ENABLE_MINIMAL_FLAG=""
fi

./configure \
  --disable-soname-versions \
  --disable-pie \
  ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
  --host="${HOST_COMPILER}" \
  --prefix="${PREFIX}" \
  --with-sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

if [ "$NDK_PLATFORM" != "$NDK_PLATFORM_COMPAT" ]; then
  egrep '^#define ' config.log | sort -u >config-def-compat.log
  echo
  echo "Configuring again for platform [${NDK_PLATFORM}]"
  echo

  ./configure \
    --disable-soname-versions \
    --disable-pie \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
    --host="${HOST_COMPILER}" \
    --prefix="${PREFIX}" \
    --with-sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

  grep -E '^#define ' config.log | sort -u >config-def.log
  if ! cmp config-def.log config-def-compat.log; then
    echo "Platform [${NDK_PLATFORM}] is not backwards-compatible with [${NDK_PLATFORM_COMPAT}]" >&2
    diff -u config-def.log config-def-compat.log >&2
    exit 1
  fi
  rm -f config-def.log config-def-compat.log
fi

NPROCESSORS=$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)
PROCESSORS=${NPROCESSORS:-3}

make clean &&
  make -j${PROCESSORS} install &&
  echo "libsodium has been installed into ${PREFIX}"
