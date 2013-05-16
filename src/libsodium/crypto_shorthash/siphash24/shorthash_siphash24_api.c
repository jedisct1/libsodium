#include "crypto_shorthash_siphash24.h"

size_t
crypto_shorthash_siphash24_bytes(void) {
    return crypto_shorthash_siphash24_BYTES;
}

const char *
crypto_shorthash_siphash24_primitive(void) {
    return "siphash24";
}
