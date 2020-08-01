#! /bin/sh
./autogen.sh -s
env CPPFLAGS="-DDEV_MODE=1" ./configure --disable-dependency-tracking
