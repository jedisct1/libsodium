#! /bin/sh

export CFLAGS="-Os -march=pentium2 -mtune=nocona"

./configure --disable-ssp && make -j3
