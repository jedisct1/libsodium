#include "crypto_hash_sha512.h"

size_t
crypto_hash_sha512_bytes(void) {
    return crypto_hash_sha512_BYTES;
}

const char *
crypto_hash_sha512_primitive(void) {
    return "sha512";
}
