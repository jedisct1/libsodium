#! /bin/sh

export CFLAGS="-O3 -fomit-frame-pointer -march=pentium2 -mtune=nocona"
export PREFIX="$(pwd)/libsodium-win32"

./configure --prefix="$PREFIX" --exec-prefix="$PREFIX" && \
make && \
make check && \
make install
