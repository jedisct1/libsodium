#! /bin/sh

if [ -z "$WASI_SYSROOT" ]; then
  export WASI_SYSROOT="/opt/wasi-sysroot"
fi

export PATH="/usr/local/opt/llvm/bin:$PATH"

export PREFIX="$(pwd)/libsodium-wasm32-wasi"

mkdir -p $PREFIX || exit 1

export CC="clang"
export CFLAGS="--target=wasm32-unknkown-wasi --sysroot=${WASI_SYSROOT} -Os"
export LDFLAGS="-s"
export NM="llvm-nm"
export AR="llvm-ar"
export RANLIB="llvm-ranlib"
export STRIP="llvm-strip"

make distclean > /dev/null

grep -q -F -- '-wasi' build-aux/config.sub || \
  sed -i -e 's/-nacl\*)/-nacl*|-wasi)/' build-aux/config.sub

if [ -z "$LIBSODIUM_FULL_BUILD" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
else
  export LIBSODIUM_ENABLE_MINIMAL_FLAG=""
fi

./configure ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
            --prefix="$PREFIX" --with-sysroot="$WASI_SYSROOT" \
            --host=wasm32-unknown-wasi --disable-ssp --disable-shared || exit 1

NPROCESSORS=$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)
PROCESSORS=${NPROCESSORS:-3}

make -j${PROCESSORS} check || exit 1
make install || exit 1

make distclean > /dev/null
