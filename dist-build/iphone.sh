#! /bin/sh

export XCODEDIR="/Applications/Xcode.app/Contents/Developer"
export BASEDIR="${XCODEDIR}/Platforms/iPhoneOS.platform/Developer"
export PATH="${BASEDIR}/usr/bin:$BASEDIR/usr/sbin:$PATH"
export SDK="${BASEDIR}/SDKs/iPhoneOS7.0.sdk"
export CFLAGS="-Oz -mthumb -arch armv7 -isysroot ${SDK}"
export LDFLAGS="-mthumb -arch armv7 -isysroot ${SDK}"
export PREFIX="$(pwd)/libsodium-ios"

./configure --host=arm-apple-darwin10 \
            --disable-shared \
            --prefix="$PREFIX" && \
make clean && \
make -j3 install && \
echo "libsodium has been installed into $PREFIX"
