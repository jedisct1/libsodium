#! /bin/sh

export CFLAGS="-Ofast -fomit-frame-pointer -m32 -march=pentium3 -mtune=westmere"
export LDFLAGS="-m32 -march=pentium3 -flto"
export PREFIX="$(pwd)/libsodium-win32"

make distclean > /dev/null

./configure --prefix="$PREFIX" --exec-prefix="$PREFIX" \
            --host=i686-w64-mingw32 && \
make && \
make check && \
make install
