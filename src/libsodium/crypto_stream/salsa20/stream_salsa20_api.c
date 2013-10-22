#include "crypto_stream_salsa20.h"

size_t
crypto_stream_salsa20_keybytes(void) {
    return crypto_stream_salsa20_KEYBYTES;
}

size_t
crypto_stream_salsa20_noncebytes(void) {
    return crypto_stream_salsa20_NONCEBYTES;
}

const char *
crypto_stream_salsa20_primitive(void) {
    return "salsa20";
}
