#! /bin/sh

export NACL_SDK_ROOT=${NACL_SDK_ROOT-"/opt/nacl_sdk/pepper_49"}
export NACL_TOOLCHAIN=${NACL_TOOLCHAIN-"${NACL_SDK_ROOT}/toolchain/mac_x86_glibc"}
export NACL_BIN=${NACL_BIN-"${NACL_TOOLCHAIN}/bin"}
export PREFIX="$(pwd)/libsodium-nativeclient-x86_64"
export PATH="${NACL_BIN}:$PATH"
export CFLAGS="-O3 -fomit-frame-pointer -fforce-addr"

mkdir -p $PREFIX || exit 1

make distclean > /dev/null

./configure --enable-minimal \
            --host=x86_64-nacl \
            --disable-ssp --without-pthreads \
            --prefix="$PREFIX" || exit 1

make -j3 check && make -j3 install || exit 1
