#! /bin/sh
#
#  Step 1.
#  Configure for base system so simulator is covered
#  
#  Step 2.
#  Make for iOS and iOS simulator
#
#  Step 3.
#  Merge libs into final version for xcode import

export PREFIX="$(pwd)/libsodium-ios"
export IOS32_PREFIX="$PREFIX/tmp/ios32"
export IOS32s_PREFIX="$PREFIX/tmp/ios32s"
export IOS64_PREFIX="$PREFIX/tmp/ios64"
export SIMULATOR32_PREFIX="$PREFIX/tmp/simulator32"
export SIMULATOR64_PREFIX="$PREFIX/tmp/simulator64"
export XCODEDIR=$(xcode-select -p)

xcode_major=$(xcodebuild -version|egrep '^Xcode '|cut -d' ' -f2|cut -d. -f1)
if [ $xcode_major -ge 8 ]; then
  export IOS_SIMULATOR_VERSION_MIN=${IOS_SIMULATOR_VERSION_MIN-"6.0.0"}
  export IOS_VERSION_MIN=${IOS_VERSION_MIN-"6.0.0"}
else
  export IOS_SIMULATOR_VERSION_MIN=${IOS_SIMULATOR_VERSION_MIN-"5.1.1"}
  export IOS_VERSION_MIN=${IOS_VERSION_MIN-"5.1.1"}
fi

mkdir -p $SIMULATOR32_PREFIX $SIMULATOR64_PREFIX $IOS32_PREFIX $IOS32s_PREFIX $IOS64_PREFIX || exit 1

# Build for the simulator
export BASEDIR="${XCODEDIR}/Platforms/iPhoneSimulator.platform/Developer"
export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
export SDK="${BASEDIR}/SDKs/iPhoneSimulator.sdk"

## i386 simulator
export CFLAGS="-O2 -arch i386 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN} -flto"
export LDFLAGS="-arch i386 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN} -flto"

make distclean > /dev/null

./configure --host=i686-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$SIMULATOR32_PREFIX" || exit 1

make -j3 install || exit 1

## x86_64 simulator
export CFLAGS="-O2 -arch x86_64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN} -flto"
export LDFLAGS="-arch x86_64 -isysroot ${SDK} -mios-simulator-version-min=${IOS_SIMULATOR_VERSION_MIN} -flto"

make distclean > /dev/null

./configure --host=x86_64-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$SIMULATOR64_PREFIX"

make -j3 install || exit 1

# Build for iOS
export BASEDIR="${XCODEDIR}/Platforms/iPhoneOS.platform/Developer"
export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
export SDK="${BASEDIR}/SDKs/iPhoneOS.sdk"

## 32-bit iOS
export CFLAGS="-O2 -mthumb -arch armv7 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"
export LDFLAGS="-mthumb -arch armv7 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"

make distclean > /dev/null

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$IOS32_PREFIX" || exit 1

make -j3 install || exit 1

## 32-bit armv7s iOS
export CFLAGS="-O2 -mthumb -arch armv7s -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"
export LDFLAGS="-mthumb -arch armv7s -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"

make distclean > /dev/null

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$IOS32s_PREFIX" || exit 1

make -j3 install || exit 1

## 64-bit iOS
export CFLAGS="-O2 -arch arm64 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"
export LDFLAGS="-arch arm64 -isysroot ${SDK} -mios-version-min=${IOS_VERSION_MIN} -flto"

make distclean > /dev/null

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$IOS64_PREFIX" || exit 1

make -j3 install || exit 1

# Create universal binary and include folder
rm -fr -- "$PREFIX/include" "$PREFIX/libsodium.a" 2> /dev/null
mkdir -p -- "$PREFIX/lib"
lipo -create \
  "$SIMULATOR32_PREFIX/lib/libsodium.a" \
  "$SIMULATOR64_PREFIX/lib/libsodium.a" \
  "$IOS32_PREFIX/lib/libsodium.a" \
  "$IOS32s_PREFIX/lib/libsodium.a" \
  "$IOS64_PREFIX/lib/libsodium.a" \
  -output "$PREFIX/lib/libsodium.a"
mv -f -- "$IOS32_PREFIX/include" "$PREFIX/"

echo
echo "libsodium has been installed into $PREFIX"
echo
file -- "$PREFIX/lib/libsodium.a"

# Cleanup
rm -rf -- "$PREFIX/tmp"
make distclean > /dev/null
