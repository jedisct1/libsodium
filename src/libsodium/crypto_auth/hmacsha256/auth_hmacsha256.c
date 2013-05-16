#include "crypto_auth_hmacsha256.h"

size_t
crypto_auth_hmacsha256_bytes(void) {
    return crypto_auth_hmacsha256_BYTES;
}

size_t
crypto_auth_hmacsha256_keybytes(void) {
    return crypto_auth_hmacsha256_KEYBYTES;
}

const char *
crypto_auth_hmacsha256_primitive(void) {
    return "hmacsha256";
}
