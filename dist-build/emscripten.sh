#! /bin/sh

export PREFIX="$(pwd)/libsodium-js"
export EXPORTED_FUNCTIONS='["_sodium_version_string"]'
export OPTFLAGS="--llvm-lto 3 -Oz"
export CFLAGS="$OPTFLAGS --pre-js=test/default/pre.js "
export LDFLAGS="-s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS}"

emconfigure ./configure --disable-shared --prefix="$PREFIX" && \
emmake make clean &&
emmake make install &&
emcc --closure 1 $OPTFLAGS $LDFLAGS \
  "${PREFIX}/lib/libsodium.a" -o "${PREFIX}/lib/libsodium.js"
