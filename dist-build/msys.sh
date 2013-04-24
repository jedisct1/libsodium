#! /bin/sh

export CFLAGS="-O3 -fomit-frame-pointer -march=pentium2 -mtune=nocona"
./configure && make && make check && make install
