#! /bin/sh
#
#  Step 1.
#  Configure for base system so simulator is covered
#  
#  Step 2.
#  Make for iOS arm7 and arm64
#
#  Step 3.
#  Merge libs into final version for xcode import

export PREFIX="$(pwd)/libsodium-ios"
export IOS_PREFIX="$PREFIX/tmp/ios"
export OSX_PREFIX="$PREFIX/tmp/osx"

mkdir -p $IOS_PREFIX $OSX_PREFIX || exit 1

# Build for OSX First
export CFLAGS="-Oz"
./configure --disable-shared \
            --enable-minimal \
            --prefix="$OSX_PREFIX"

make clean > /dev/null && make -j3 check && make -j3 install

# Cleanup
make distclean > /dev/null

# Build for iOS
export XCODEDIR=$(xcode-select -p)
export BASEDIR="${XCODEDIR}/Platforms/iPhoneOS.platform/Developer"
export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
export SDK="${BASEDIR}/SDKs/iPhoneOS.sdk"
export IPHONEOS_VERSION_MIN="5.1.1"
export CFLAGS="-Oz -mthumb -arch armv7 -arch arm64 -isysroot ${SDK} -miphoneos-version-min=${IPHONEOS_VERSION_MIN}"
export LDFLAGS="-mthumb -arch armv7 -arch arm64 -isysroot ${SDK} -miphoneos-version-min=${IPHONEOS_VERSION_MIN}"

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --enable-minimal \
            --prefix="$IOS_PREFIX" 

make clean > /dev/null && make -j3 install

# Create universal binary and include folder
rm -fr -- "$PREFIX/include" "$PREFIX/libsodium.a" 2> /dev/null
mkdir -p -- "$PREFIX"
lipo -create "$IOS_PREFIX/lib/libsodium.a" "$OSX_PREFIX/lib/libsodium.a" -output "$PREFIX/libsodium.a"
mv -f -- "$IOS_PREFIX/include" "$PREFIX/"

echo
echo "libsodium has been installed into $PREFIX"
echo
file -- "$PREFIX/libsodium.a"

# Cleanup
rm -rf -- "$PREFIX/tmp"
make distclean > /dev/null
