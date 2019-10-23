#! /bin/sh

if ! command -v wasienv >/dev/null; then
  echo "Wasienv needs to be installed - https://github.com/wasienv/wasienv" >&2
  exit 1
fi

export PREFIX="$(pwd)/libsodium-wasm32-wasi"

mkdir -p $PREFIX || exit 1

export CFLAGS="-DED25519_NONDETERMINISTIC=1 -O2"
export LDFLAGS="-s -Wl,--no-threads"

make distclean > /dev/null

# This is necessary for Frankeinstein Linux systems, where people mix a base
# system containing old tools (here: old autotools) with backported or
# hand-compiled recent tools (here: LLVM)
grep -q -F -- 'wasi' build-aux/config.sub || \
  sed -i -e 's/-nacl\*)/-nacl*|-wasi)/' build-aux/config.sub

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
