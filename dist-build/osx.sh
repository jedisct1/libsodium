#! /bin/sh

export PREFIX="$(pwd)/libsodium-osx"
export OSX_VERSION_MIN=${OSX_VERSION_MIN-"10.8"}
export OSX_CPU_ARCH=${OSX_CPU_ARCH-"core2"}

mkdir -p $PREFIX || exit 1

export MACOS_X86_PREFIX="$PREFIX/macOS_x86"
export MACOS_i386_PREFIX="$PREFIX/macOS_i386"

# Build for x86_64
export CFLAGS="-arch x86_64 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH} -O2 -g -flto"
export LDFLAGS="-arch x86_64 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH} -flto"

make distclean > /dev/null

./configure --enable-minimal \
            --prefix="$MACOS_X86_PREFIX" || exit 1

make -j3 check && make -j3 install || exit 1

# Build for i386
export CFLAGS="-arch i386 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH} -O2 -g -flto"
export LDFLAGS="-arch i386 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH} -flto"

make distclean > /dev/null

./configure --enable-minimal \
            --prefix="$MACOS_i386_PREFIX" || exit 1

make -j3 check && make -j3 install || exit 1

make distclean > /dev/null

# Create universal binary and include folder
rm -fr -- "$PREFIX/include" "$PREFIX/libsodium.a" 2> /dev/null
mkdir -p -- "$PREFIX/lib"
lipo -create \
  "$MACOS_i386_PREFIX/lib/libsodium.a" \
  "$MACOS_X86_PREFIX/lib/libsodium.a" \
  -output "$PREFIX/lib/libsodium.a"
mv -f -- "$MACOS_X86_PREFIX/include" "$PREFIX/"

echo
echo "libsodium has been installed into $PREFIX"
echo
file -- "$PREFIX/lib/libsodium.a"
