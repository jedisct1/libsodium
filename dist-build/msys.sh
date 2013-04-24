#! /bin/sh

export CFLAGS="-O3 -fomit-frame-pointer -march=pentium2 -mtune=nocona"
./configure --disable-ssp && make && make check
