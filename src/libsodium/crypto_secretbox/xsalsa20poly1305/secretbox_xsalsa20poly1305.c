#include "crypto_secretbox_xsalsa20poly1305.h"

size_t
crypto_secretbox_xsalsa20poly1305_keybytes(void) {
    return crypto_secretbox_xsalsa20poly1305_KEYBYTES;
}

size_t
crypto_secretbox_xsalsa20poly1305_noncebytes(void) {
    return crypto_secretbox_xsalsa20poly1305_NONCEBYTES;
}

size_t
crypto_secretbox_xsalsa20poly1305_zerobytes(void) {
    return crypto_secretbox_xsalsa20poly1305_ZEROBYTES;
}

size_t
crypto_secretbox_xsalsa20poly1305_boxzerobytes(void) {
    return crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES;
}

const char *
crypto_secretbox_xsalsa20poly1305_primitive(void) {
    return "xsalsa20poly1305";
}
