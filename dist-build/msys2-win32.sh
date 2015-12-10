#! /bin/sh

export CFLAGS="-O3 -fomit-frame-pointer -m32 -march=pentium2 -mtune=nocona"
export PREFIX="$(pwd)/libsodium-win32"

./configure --prefix="$PREFIX" --exec-prefix="$PREFIX" \
            --host=i686-w64-mingw32 && \
make clean && \
make && \
make check && \
make install
