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
export IOS_PREFIX="$PREFIX/tmp/osx"
export OSX_PREFIX="$PREFIX/tmp/ios"

mkdir -p $IOS_PREFIX && mkdir -p $OSX_PREFIX

# Build for OSX First

./configure --prefix="$OSX_PREFIX" 
make clean && make -j3 install

# Cleanup
rm Makefile

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

make clean && make -j3 install 

# Create universal binary and include folder
lipo -create "$IOS_PREFIX/lib/libsodium.a" "$OSX_PREFIX/lib/libsodium.a" -output "$PREFIX/libsodium.a" 
cp -r "$IOS_PREFIX/include" "$PREFIX/include"

# Cleanup
rm -rf "$PREFIX/tmp"
rm Makefile
