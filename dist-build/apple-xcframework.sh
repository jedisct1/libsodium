#! /bin/sh

export PREFIX="$(pwd)/libsodium-apple"
export MACOS_ARM64_PREFIX="${PREFIX}/tmp/macos-arm64"
export MACOS_X86_64_PREFIX="${PREFIX}/tmp/macos-x86_64"
export IOS32_PREFIX="${PREFIX}/tmp/ios32"
export IOS32s_PREFIX="${PREFIX}/tmp/ios32s"
export IOS64_PREFIX="${PREFIX}/tmp/ios64"
export IOS_SIMULATOR_ARM64_PREFIX="${PREFIX}/tmp/ios-simulator-arm64"
export IOS_SIMULATOR_I386_PREFIX="${PREFIX}/tmp/ios-simulator-i386"
export IOS_SIMULATOR_X86_64_PREFIX="${PREFIX}/tmp/ios-simulator-x86_64"
export WATCHOS32_PREFIX="${PREFIX}/tmp/watchos32"
export WATCHOS64_32_PREFIX="${PREFIX}/tmp/watchos64_32"
export WATCHOS64_PREFIX="${PREFIX}/tmp/watchos64"
export WATCHOS_SIMULATOR_ARM64_PREFIX="${PREFIX}/tmp/watchos-simulator-arm64"
export WATCHOS_SIMULATOR_I386_PREFIX="${PREFIX}/tmp/watchos-simulator-i386"
export WATCHOS_SIMULATOR_X86_64_PREFIX="${PREFIX}/tmp/watchos-simulator-x86_64"
export TVOS_PREFIX="${PREFIX}/tmp/tvos"
export TVOS_SIMULATOR_ARM64_PREFIX="${PREFIX}/tmp/tvos-simulator-arm64"
export TVOS_SIMULATOR_X86_64_PREFIX="${PREFIX}/tmp/tvos-simulator-x86_64"
export VISIONOS_PREFIX="${PREFIX}/tmp/visionos"
export VISIONOS_SIMULATOR_PREFIX="${PREFIX}/tmp/visionos-simulator"
export CATALYST_ARM64_PREFIX="${PREFIX}/tmp/catalyst-arm64"
export CATALYST_X86_64_PREFIX="${PREFIX}/tmp/catalyst-x86_64"
export LOG_FILE="${PREFIX}/tmp/build_log"
export XCODEDIR="$(xcode-select -p)"

export MACOS_VERSION_MIN=${MACOS_VERSION_MIN-"10.10"}
export IOS_VERSION_MIN=${IOS_VERSION_MIN-"9.0.0"}
export IOS_SIMULATOR_VERSION_MIN=${IOS_SIMULATOR_VERSION_MIN-$IOS_VERSION_MIN}
export WATCHOS_VERSION_MIN=${WATCHOS_VERSION_MIN-"4.0.0"}
export WATCHOS_SIMULATOR_VERSION_MIN=${WATCHOS_SIMULATOR_VERSION_MIN-$WATCHOS_VERSION_MIN}
export TVOS_VERSION_MIN=${TVOS_VERSION_MIN-"9.0.0"}
export TVOS_SIMULATOR_VERSION_MIN=${TVOS_SIMULATOR_VERSION_MIN-$TVOS_VERSION_MIN}

echo
echo "Warnings related to headers being present but not usable are due to functions"
echo "that didn't exist in the specified minimum iOS version level."
echo "They can be safely ignored."
echo
echo "Define the LIBSODIUM_FULL_BUILD environment variable to build the full"
echo "library (including all deprecated/undocumented/low-level functions)."
echo
echo "Define the LIBSODIUM_SKIP_SIMULATORS environment variable to skip building"
echo "the simulators libraries (iOS, watchOS, tvOS, visionOS simulators)."
echo

if [ -z "$LIBSODIUM_FULL_BUILD" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
else
  export LIBSODIUM_ENABLE_MINIMAL_FLAG=""
fi

IOS32_SUPPORTED=false
[ "$(echo "$IOS_VERSION_MIN" | cut -d'.' -f1)" -lt "11" ] && IOS32_SUPPORTED=true

I386_SIMULATOR_SUPPORTED=false
[ "$(echo "$IOS_SIMULATOR_VERSION_MIN" | cut -d'.' -f1)" -lt "11" ] && I386_SIMULATOR_SUPPORTED=true

VISIONOS_SUPPORTED=false
[ -d "${XCODEDIR}/Platforms/XROS.platform" ] && VISIONOS_SUPPORTED=true

NPROCESSORS=$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)
PROCESSORS=${NPROCESSORS:-3}

swift_module_map() {
  echo 'module Clibsodium {'
  echo '    header "sodium.h"'
  echo '    export *'
  echo '}'
}

build_macos() {
  export BASEDIR="${XCODEDIR}/Platforms/MacOSX.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"

  ## macOS arm64
  export CFLAGS="-O3 -arch arm64 -mmacosx-version-min=${MACOS_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -mmacosx-version-min=${MACOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$MACOS_ARM64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## macOS x86_64
  export CFLAGS="-O3 -arch x86_64 -mmacosx-version-min=${MACOS_VERSION_MIN}"
  export LDFLAGS="-arch x86_64 -mmacosx-version-min=${MACOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=x86_64-apple-darwin23 --prefix="$MACOS_X86_64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_ios() {
  export BASEDIR="${XCODEDIR}/Platforms/iPhoneOS.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/iPhoneOS.sdk"

  if [ "$IOS32_SUPPORTED" = true ]; then
    ## 32-bit iOS
    export CFLAGS="-O3 -mthumb -arch armv7 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"
    export LDFLAGS="-mthumb -arch armv7 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"

    make distclean >/dev/null 2>&1
    ./configure --host=arm-apple-darwin23 --prefix="$IOS32_PREFIX" \
      ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
    make -j${PROCESSORS} install || exit 1

    ## 32-bit armv7s iOS
    export CFLAGS="-O3 -mthumb -arch armv7s -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"
    export LDFLAGS="-mthumb -arch armv7s -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"

    make distclean >/dev/null 2>&1
    ./configure --host=arm-apple-darwin23 --prefix="$IOS32s_PREFIX" \
      ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
    make -j${PROCESSORS} install || exit 1
  fi

  ## 64-bit iOS
  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$IOS64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_ios_simulator() {
  export BASEDIR="${XCODEDIR}/Platforms/iPhoneSimulator.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/iPhoneSimulator.sdk"

  ## arm64 simulator
  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$IOS_SIMULATOR_ARM64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  if [ "$I386_SIMULATOR_SUPPORTED" = true ]; then
    ## i386 simulator
    export CFLAGS="-O3 -arch i386 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"
    export LDFLAGS="-arch i386 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"

    make distclean >/dev/null 2>&1
    ./configure --host=i686-apple-darwin23 --prefix="$IOS_SIMULATOR_I386_PREFIX" \
      ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
    make -j${PROCESSORS} install || exit 1
  fi

  ## x86_64 simulator
  export CFLAGS="-O3 -arch x86_64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch x86_64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=x86_64-apple-darwin23 --prefix="$IOS_SIMULATOR_X86_64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG}
  make -j${PROCESSORS} install || exit 1
}

build_watchos() {
  export BASEDIR="${XCODEDIR}/Platforms/WatchOS.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/WatchOS.sdk"

  # 32-bit watchOS
  export CFLAGS="-O3 -mthumb -arch armv7k -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"
  export LDFLAGS="-mthumb -arch armv7k -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=arm-apple-darwin23 --prefix="$WATCHOS32_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## 64-bit arm64_32 watchOS
  export CFLAGS="-O3 -mthumb -arch arm64_32 -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"
  export LDFLAGS="-mthumb -arch arm64_32 -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$WATCHOS64_32_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## 64-bit arm64 watchOS
  export CFLAGS="-O3 -mthumb -arch arm64 -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"
  export LDFLAGS="-mthumb -arch arm64 -isysroot ${SDK} -mwatchos-version-min=${WATCHOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$WATCHOS64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_watchos_simulator() {
  export BASEDIR="${XCODEDIR}/Platforms/WatchSimulator.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/WatchSimulator.sdk"

  ## arm64 simulator
  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$WATCHOS_SIMULATOR_ARM64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## i386 simulator
  export CFLAGS="-O3 -arch i386 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch i386 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=i686-apple-darwin23 --prefix="$WATCHOS_SIMULATOR_I386_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## x86_64 simulator
  export CFLAGS="-O3 -arch x86_64 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch x86_64 -isysroot ${SDK} -mwatchos-simulator-version-min=${WATCHOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=x86_64-apple-darwin23 --prefix="$WATCHOS_SIMULATOR_X86_64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_tvos() {
  export BASEDIR="${XCODEDIR}/Platforms/AppleTVOS.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/AppleTVOS.sdk"

  ## 64-bit tvOS
  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK} -mtvos-version-min=${TVOS_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK} -mtvos-version-min=${TVOS_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$TVOS_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_tvos_simulator() {
  export BASEDIR="${XCODEDIR}/Platforms/AppleTVSimulator.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/AppleTVSimulator.sdk"

  ## arm64 simulator
  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK} -mtvos-simulator-version-min=${TVOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK} -mtvos-simulator-version-min=${TVOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$TVOS_SIMULATOR_ARM64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## x86_64 simulator
  export CFLAGS="-O3 -arch x86_64 -isysroot ${SDK} -mtvos-simulator-version-min=${TVOS_SIMULATOR_VERSION_MIN}"
  export LDFLAGS="-arch x86_64 -isysroot ${SDK} -mtvos-simulator-version-min=${TVOS_SIMULATOR_VERSION_MIN}"

  make distclean >/dev/null 2>&1
  ./configure --host=x86_64-apple-darwin23 --prefix="$TVOS_SIMULATOR_X86_64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG}
  make -j${PROCESSORS} install || exit 1
}

build_visionos() {
  export BASEDIR="${XCODEDIR}/Platforms/XROS.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/XROS.sdk"

  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$VISIONOS_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_visionos_simulator() {
  export BASEDIR="${XCODEDIR}/Platforms/XRSimulator.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/XRSimulator.sdk"

  export CFLAGS="-O3 -arch arm64 -isysroot ${SDK}"
  export LDFLAGS="-arch arm64 -isysroot ${SDK}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-darwin23 --prefix="$VISIONOS_SIMULATOR_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

build_catalyst() {
  export BASEDIR="${XCODEDIR}/Platforms/MacOSX.platform/Developer"
  export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
  export SDK="${BASEDIR}/SDKs/MacOSX.sdk"

  ## arm64 catalyst
  export CFLAGS="-O3 -arch arm64 -target arm64-apple-ios13.1-macabi -isysroot ${SDK}"
  export LDFLAGS="-arch arm64 -target arm64-apple-ios13.1-macabi -isysroot ${SDK}"

  make distclean >/dev/null 2>&1
  ./configure --host=aarch64-apple-ios --prefix="$CATALYST_ARM64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1

  ## x86_64 catalyst
  export CFLAGS="-O3 -arch x86_64 -target x86_64-apple-ios13.1-macabi -isysroot ${SDK}"
  export LDFLAGS="-arch x86_64 -target x86_64-apple-ios13.1-macabi -isysroot ${SDK}"

  make distclean >/dev/null 2>&1
  ./configure --host=x86_64-apple-ios --prefix="$CATALYST_X86_64_PREFIX" \
    ${LIBSODIUM_ENABLE_MINIMAL_FLAG} || exit 1
  make -j${PROCESSORS} install || exit 1
}

mkdir -p "${PREFIX}/tmp"

echo "Building for macOS..."
build_macos >"$LOG_FILE" 2>&1 || exit 1
echo "Building for iOS..."
build_ios >"$LOG_FILE" 2>&1 || exit 1
echo "Building for watchOS..."
build_watchos >"$LOG_FILE" 2>&1 || exit 1
echo "Building for tvOS..."
build_tvos >"$LOG_FILE" 2>&1 || exit 1
echo "Building for Catalyst..."
build_catalyst >"$LOG_FILE" 2>&1 || exit 1
if [ "$VISIONOS_SUPPORTED" = true ]; then
  echo "Building for visionOS..."
  build_visionos >"$LOG_FILE" 2>&1 || exit 1
fi

if [ -z "$LIBSODIUM_SKIP_SIMULATORS" ]; then
  echo "Building for the iOS simulator..."
  build_ios_simulator >"$LOG_FILE" 2>&1 || exit 1
  echo "Building for the watchOS simulator..."
  build_watchos_simulator >"$LOG_FILE" 2>&1 || exit 1
  echo "Building for the tvOS simulator..."
  build_tvos_simulator >"$LOG_FILE" 2>&1 || exit 1
  if [ "$VISIONOS_SUPPORTED" = true ]; then
    echo "Building for the visionOS simulator..."
    build_visionos_simulator >"$LOG_FILE" 2>&1 || exit 1
  fi
else
  echo "[Skipping the simulators]"
fi

echo "Adding the Clibsodium module map for Swift..."

find "$PREFIX" -name "include" -type d -print | while read -r f; do
  swift_module_map >"${f}/module.modulemap"
done

echo "Bundling macOS targets..."

mkdir -p "${PREFIX}/macos/lib"
cp -a "${MACOS_X86_64_PREFIX}/include" "${PREFIX}/macos/"
for ext in a dylib; do
  lipo -create \
    "${MACOS_ARM64_PREFIX}/lib/libsodium.${ext}" \
    "${MACOS_X86_64_PREFIX}/lib/libsodium.${ext}" \
    -output "${PREFIX}/macos/lib/libsodium.${ext}"
done

echo "Bundling iOS targets..."

mkdir -p "${PREFIX}/ios/lib"
cp -a "${IOS64_PREFIX}/include" "${PREFIX}/ios/"
for ext in a dylib; do
  LIBRARY_PATHS="$IOS64_PREFIX/lib/libsodium.${ext}"
  if [ "$IOS32_SUPPORTED" = true ]; then
    LIBRARY_PATHS="$LIBRARY_PATHS $IOS32_PREFIX/lib/libsodium.${ext}"
    LIBRARY_PATHS="$LIBRARY_PATHS $IOS32s_PREFIX/lib/libsodium.${ext}"
  fi
  lipo -create \
    ${LIBRARY_PATHS} \
    -output "$PREFIX/ios/lib/libsodium.${ext}"
done

echo "Bundling watchOS targets..."

mkdir -p "${PREFIX}/watchos/lib"
cp -a "${WATCHOS64_32_PREFIX}/include" "${PREFIX}/watchos/"
for ext in a dylib; do
  lipo -create \
    "${WATCHOS32_PREFIX}/lib/libsodium.${ext}" \
    "${WATCHOS64_32_PREFIX}/lib/libsodium.${ext}" \
    "${WATCHOS64_PREFIX}/lib/libsodium.${ext}" \
    -output "${PREFIX}/watchos/lib/libsodium.${ext}"
done

echo "Bundling tvOS targets..."

mkdir -p "${PREFIX}/tvos/lib"
cp -a "${TVOS_PREFIX}/include" "${PREFIX}/tvos/"
for ext in a dylib; do
  lipo -create \
    "$TVOS_PREFIX/lib/libsodium.${ext}" \
    -output "$PREFIX/tvos/lib/libsodium.${ext}"
done

if [ "$VISIONOS_SUPPORTED" = true ]; then
  echo "Bundling visionOS targets..."

  mkdir -p "${PREFIX}/visionos/lib"
  cp -a "${VISIONOS_PREFIX}/include" "${PREFIX}/visionos/"
  for ext in a dylib; do
    lipo -create \
      "$VISIONOS_PREFIX/lib/libsodium.${ext}" \
      -output "$PREFIX/visionos/lib/libsodium.${ext}"
  done
fi

echo "Bundling Catalyst targets..."

mkdir -p "${PREFIX}/catalyst/lib"
cp -a "${CATALYST_X86_64_PREFIX}/include" "${PREFIX}/catalyst/"
for ext in a dylib; do
  if [ ! -f "${CATALYST_X86_64_PREFIX}/lib/libsodium.${ext}" ]; then
    continue
  fi
  lipo -create \
    "${CATALYST_ARM64_PREFIX}/lib/libsodium.${ext}" \
    "${CATALYST_X86_64_PREFIX}/lib/libsodium.${ext}" \
    -output "${PREFIX}/catalyst/lib/libsodium.${ext}"
done

if [ -z "$LIBSODIUM_SKIP_SIMULATORS" ]; then
  echo "Bundling iOS simulators..."

  mkdir -p "${PREFIX}/ios-simulators/lib"
  cp -a "${IOS_SIMULATOR_X86_64_PREFIX}/include" "${PREFIX}/ios-simulators/"
  for ext in a dylib; do
    LIBRARY_PATHS="${IOS_SIMULATOR_ARM64_PREFIX}/lib/libsodium.${ext}"
    LIBRARY_PATHS="$LIBRARY_PATHS ${IOS_SIMULATOR_X86_64_PREFIX}/lib/libsodium.${ext}"
    if [ "$I386_SIMULATOR_SUPPORTED" = true ]; then
      LIBRARY_PATHS="$LIBRARY_PATHS ${IOS_SIMULATOR_I386_PREFIX}/lib/libsodium.${ext}"
    fi
    lipo -create \
      ${LIBRARY_PATHS} \
      -output "${PREFIX}/ios-simulators/lib/libsodium.${ext}" || exit 1
  done

  echo "Bundling watchOS simulators..."

  mkdir -p "${PREFIX}/watchos-simulators/lib"
  cp -a "${WATCHOS_SIMULATOR_X86_64_PREFIX}/include" "${PREFIX}/watchos-simulators/"
  for ext in a dylib; do
    lipo -create \
      "${WATCHOS_SIMULATOR_ARM64_PREFIX}/lib/libsodium.${ext}" \
      "${WATCHOS_SIMULATOR_I386_PREFIX}/lib/libsodium.${ext}" \
      "${WATCHOS_SIMULATOR_X86_64_PREFIX}/lib/libsodium.${ext}" \
      -output "${PREFIX}/watchos-simulators/lib/libsodium.${ext}"
  done

  echo "Bundling tvOS simulators..."

  mkdir -p "${PREFIX}/tvos-simulators/lib"
  cp -a "${TVOS_SIMULATOR_X86_64_PREFIX}/include" "${PREFIX}/tvos-simulators/"
  for ext in a dylib; do
    lipo -create \
      "${TVOS_SIMULATOR_ARM64_PREFIX}/lib/libsodium.${ext}" \
      "${TVOS_SIMULATOR_X86_64_PREFIX}/lib/libsodium.${ext}" \
      -output "${PREFIX}/tvos-simulators/lib/libsodium.${ext}" || exit 1
  done

  if [ "$VISIONOS_SUPPORTED" = true ]; then
    echo "Bundling visionOS simulators..."

    mkdir -p "${PREFIX}/visionos-simulators/lib"
    cp -a "${VISIONOS_SIMULATOR_PREFIX}/include" "${PREFIX}/visionos-simulators/"
    for ext in a dylib; do
      lipo -create \
        "${VISIONOS_SIMULATOR_PREFIX}/lib/libsodium.${ext}" \
        -output "${PREFIX}/visionos-simulators/lib/libsodium.${ext}" || exit 1
    done
  fi
fi

echo "Creating Clibsodium.xcframework..."

rm -rf "${PREFIX}/Clibsodium.xcframework"

XCFRAMEWORK_ARGS=""
for f in macos ios watchos tvos visionos catalyst; do
  if [ "$VISIONOS_SUPPORTED" = false ] && [ "$f" = "visionos" ]; then
    continue
  fi
  XCFRAMEWORK_ARGS="${XCFRAMEWORK_ARGS} -library $(readlink -f ${PREFIX}/${f}/lib/libsodium.a)"
  XCFRAMEWORK_ARGS="${XCFRAMEWORK_ARGS} -headers $(readlink -f ${PREFIX}/${f}/include)"
done
if [ -z "$LIBSODIUM_SKIP_SIMULATORS" ]; then
  for f in ios-simulators watchos-simulators tvos-simulators visionos-simulators; do
    if [ "$VISIONOS_SUPPORTED" = false ] && [ "$f" = "visionos-simulators" ]; then
      continue
    fi
    XCFRAMEWORK_ARGS="${XCFRAMEWORK_ARGS} -library $(readlink -f ${PREFIX}/${f}/lib/libsodium.a)"
    XCFRAMEWORK_ARGS="${XCFRAMEWORK_ARGS} -headers $(readlink -f ${PREFIX}/${f}/include)"
  done
fi
xcodebuild -create-xcframework \
  ${XCFRAMEWORK_ARGS} \
  -output "${PREFIX}/Clibsodium.xcframework" >/dev/null

ls -ld -- "$PREFIX"
ls -l -- "$PREFIX"
ls -l -- "$PREFIX/Clibsodium.xcframework"

echo "Done!"

# Cleanup
rm -rf -- "$PREFIX/tmp"
make distclean >/dev/null
