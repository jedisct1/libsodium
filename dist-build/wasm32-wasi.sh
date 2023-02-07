#! /bin/sh

export PATH="/opt/zig/bin:/opt/zig:/opt/homebrew/bin:$PATH"

export PREFIX="$(pwd)/libsodium-wasm32-wasi"

mkdir -p $PREFIX || exit 1

export CC="zig cc"
export CFLAGS="--target=wasm32-wasi -O3"
export LDFLAGS="-s"
export AR="zig ar"
export RANLIB="zig ranlib"

make distclean >/dev/null

if [ "x$1" = "x--bench" ]; then
  export BENCHMARKS=1
  export CPPFLAGS="-DBENCHMARKS -DITERATIONS=200"
else
  export CPPFLAGS="-DED25519_NONDETERMINISTIC=1"
fi

if [ -n "$LIBSODIUM_MINIMAL_BUILD" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
else
  export LIBSODIUM_ENABLE_MINIMAL_FLAG=""
fi

if ! ./configure ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
  --prefix="$PREFIX" \
  --host=wasm32-wasi \
  --disable-ssp --disable-shared --without-pthreads; then
  cat config.log
  exit 1
fi

NPROCESSORS=$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)
PROCESSORS=${NPROCESSORS:-3}

if [ -z "$BENCHMARKS" ]; then
  make -j${PROCESSORS} check && make install && make distclean >/dev/null
else
  make -j${PROCESSORS} && make check
fi
