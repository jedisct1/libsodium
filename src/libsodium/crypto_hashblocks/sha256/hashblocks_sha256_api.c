#include "crypto_hashblocks_sha256.h"

size_t
crypto_hashblocks_sha256_statebytes(void) {
    return crypto_hashblocks_sha256_STATEBYTES;
}

size_t
crypto_hashblocks_sha256_blockbytes(void) {
    return crypto_hashblocks_sha256_BLOCKBYTES;
}

const char *
crypto_hashblocks_sha256_primitive(void) {
    return "sha256";
}
