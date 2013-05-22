#include "crypto_hashblocks_sha512.h"

size_t
crypto_hashblocks_sha512_statebytes(void) {
    return crypto_hashblocks_sha512_STATEBYTES;
}

size_t
crypto_hashblocks_sha512_blockbytes(void) {
    return crypto_hashblocks_sha512_BLOCKBYTES;
}

const char *
crypto_hashblocks_sha512_primitive(void) {
    return "sha512";
}
