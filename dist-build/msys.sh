#! /bin/sh

export CFLAGS="-O3 -fomit-frame-pointer -march=pentium2 -mtune=nocona"
./configure --with-included-ltdl && make && make check && make install
