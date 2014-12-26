#! /bin/sh
#
#  Step 1.
#  Configure for base system so simulator is covered
#  
#  Step 2.
#  Make for iOS armv7, armv7s and arm64
#
#  Step 3.
#  Merge libs into final version for xcode import

export PREFIX="$(pwd)/libsodium-ios"
export IOS_PREFIX="$PREFIX/tmp/ios"
export OSX32_PREFIX="$PREFIX/tmp/osx32"
export OSX64_PREFIX="$PREFIX/tmp/osx64"

mkdir -p $IOS_PREFIX $OSX32_PREFIX $OSX64_PREFIX || exit 1

# Build for OSX32 First
export CFLAGS="-Oz -arch i386"
./configure --disable-shared \
            --enable-minimal \
            --prefix="$OSX32_PREFIX" || exit 1

make clean > /dev/null && make -j3 check && make -j3 install || exit 1

# Cleanup
make distclean > /dev/null

# Build for OSX64 Then
export CFLAGS="-Oz -arch x86_64"
./configure --disable-shared \
            --enable-minimal \
            --prefix="$OSX64_PREFIX"

make clean > /dev/null && make -j3 check && make -j3 install || exit 1

# Cleanup
make distclean > /dev/null || exit 1

# Build for iOS
export XCODEDIR=$(xcode-select -p)
export BASEDIR="${XCODEDIR}/Platforms/iPhoneOS.platform/Developer"
export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
export SDK="${BASEDIR}/SDKs/iPhoneOS.sdk"
export IPHONEOS_VERSION_MIN=${IPHONEOS_VERSION_MIN-"5.1.1"}
export CFLAGS="-Oz -mthumb -arch armv7 -arch armv7s -arch arm64 -isysroot ${SDK} -miphoneos-version-min=${IPHONEOS_VERSION_MIN}"
export LDFLAGS="-mthumb -arch armv7 -arch armv7s -arch arm64 -isysroot ${SDK} -miphoneos-version-min=${IPHONEOS_VERSION_MIN}"

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$IOS_PREFIX" || exit 1

make clean > /dev/null && make -j3 install || exit 1

# Create universal binary and include folder
rm -fr -- "$PREFIX/include" "$PREFIX/libsodium.a" 2> /dev/null
mkdir -p -- "$PREFIX"
lipo -create "$IOS_PREFIX/lib/libsodium.a" "$OSX32_PREFIX/lib/libsodium.a" "$OSX64_PREFIX/lib/libsodium.a" -output "$PREFIX/libsodium.a"
mv -f -- "$IOS_PREFIX/include" "$PREFIX/"

echo
echo "libsodium has been installed into $PREFIX"
echo
file -- "$PREFIX/libsodium.a"

# Cleanup
rm -rf -- "$PREFIX/tmp"
make distclean > /dev/null
