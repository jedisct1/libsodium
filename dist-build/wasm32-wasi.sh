#! /bin/sh

if ! command -v wasienv >/dev/null; then
  echo "Wasienv needs to be installed - https://github.com/wasienv/wasienv" >&2
  exit 1
fi

export PREFIX="$(pwd)/libsodium-wasm32-wasi"

mkdir -p $PREFIX || exit 1

export CFLAGS="-DED25519_NONDETERMINISTIC=1 -O2"

make distclean > /dev/null

if [ "x$1" = "x--bench" ]; then
  export BENCHMARKS=1
  export CPPFLAGS="-DBENCHMARKS -DITERATIONS=100"
fi

if [ -n "$LIBSODIUM_MINIMAL_BUILD" ]; then
  export LIBSODIUM_ENABLE_MINIMAL_FLAG="--enable-minimal"
else
  export LIBSODIUM_ENABLE_MINIMAL_FLAG=""
fi

wasiconfigure ./configure ${LIBSODIUM_ENABLE_MINIMAL_FLAG} \
            --prefix="$PREFIX" \
            --disable-ssp --disable-shared || exit 1

NPROCESSORS=$(getconf NPROCESSORS_ONLN 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)
PROCESSORS=${NPROCESSORS:-3}

if [ -z "$BENCHMARKS" ]; then
  make -j${PROCESSORS} check && make install && make distclean > /dev/null
else
  make -j${PROCESSORS} && make check
fi
