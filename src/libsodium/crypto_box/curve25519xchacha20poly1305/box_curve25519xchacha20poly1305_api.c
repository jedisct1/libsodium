#include "crypto_box_curve25519xchacha20poly1305.h"

size_t
crypto_box_curve25519xchacha20poly1305_seedbytes(void) {
    return crypto_box_curve25519xchacha20poly1305_SEEDBYTES;
}

size_t
crypto_box_curve25519xchacha20poly1305_publickeybytes(void) {
    return crypto_box_curve25519xchacha20poly1305_PUBLICKEYBYTES;
}

size_t
crypto_box_curve25519xchacha20poly1305_secretkeybytes(void) {
    return crypto_box_curve25519xchacha20poly1305_SECRETKEYBYTES;
}

size_t
crypto_box_curve25519xchacha20poly1305_beforenmbytes(void) {
    return crypto_box_curve25519xchacha20poly1305_BEFORENMBYTES;
}

size_t
crypto_box_curve25519xchacha20poly1305_noncebytes(void) {
    return crypto_box_curve25519xchacha20poly1305_NONCEBYTES;
}

size_t
crypto_box_curve25519xchacha20poly1305_macbytes(void) {
    return crypto_box_curve25519xchacha20poly1305_MACBYTES;
}
