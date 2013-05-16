#include "crypto_sign_ed25519.h"

size_t
crypto_sign_ed25519_bytes(void) {
    return crypto_sign_ed25519_BYTES;
}

size_t
crypto_sign_ed25519_publickeybytes(void) {
    return crypto_sign_ed25519_PUBLICKEYBYTES;
}

size_t
crypto_sign_ed25519_secretkeybytes(void) {
    return crypto_sign_ed25519_SECRETKEYBYTES;
}

const char *
crypto_sign_ed25519_primitive(void) {
    return "ed25519";
}
