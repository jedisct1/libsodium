#! /bin/sh

export NACL_SDK_ROOT=${NACL_SDK_ROOT-"/opt/nacl_sdk/pepper_49"}
export NACL_TOOLCHAIN=${NACL_TOOLCHAIN-"${NACL_SDK_ROOT}/toolchain/mac_pnacl"}
export NACL_BIN=${NACL_BIN-"${NACL_TOOLCHAIN}/bin"}
export PREFIX="$(pwd)/libsodium-nativeclient"
export PATH="${NACL_BIN}:$PATH"
export AR=${AR-"pnacl-ar"}
export AS=${AS-"pnacl-as"}
export CC=${CC-"pnacl-clang"}
export LD=${LD-"pnacl-ld"}
export NM=${NM-"pnacl-nm"}
export RANLIB=${RANLIB-"pnacl-ranlib"}
export PNACL_FINALIZE=${PNACL_FINALIZE-"pnacl-finalize"}
export PNACL_TRANSLATE=${PNACL_TRANSLATE-"pnacl-translate"}
export CFLAGS="-O3 -fomit-frame-pointer -fforce-addr"

mkdir -p $PREFIX || exit 1

make distclean > /dev/null

./configure --enable-minimal \
            --host=nacl \
            --disable-ssp --without-pthreads \
            --prefix="$PREFIX" || exit 1

make -j3 check && make -j3 install || exit 1
