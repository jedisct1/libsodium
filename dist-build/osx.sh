#! /bin/sh

export PREFIX="$(pwd)/libsodium-osx"
export OSX_VERSION_MIN=${OSX_VERSION_MIN-"10.8"}
export OSX_CPU_ARCH=${OSX_CPU_ARCH-"core2"}

mkdir -p $PREFIX || exit 1

export CFLAGS="-arch x86_64 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH} -O2 -g"
export LDFLAGS="-arch x86_64 -mmacosx-version-min=${OSX_VERSION_MIN} -march=${OSX_CPU_ARCH}"

make distclean > /dev/null

if [ -z "$LIBSODIUM_ENABLE_MINIMAL_FLAG" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
fi

./configure ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
            --prefix="$PREFIX" || exit 1

make -j3 check && make -j3 install || exit 1

# Cleanup
make distclean > /dev/null
