#include "crypto_hash_sha256.h"

size_t
crypto_hash_sha256_bytes(void) {
    return crypto_hash_sha256_BYTES;
}

const char *
crypto_hash_sha256_primitive(void) {
    return "sha256";
}
