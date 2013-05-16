#include "crypto_stream_salsa2012.h"

size_t
crypto_stream_salsa2012_keybytes(void) {
    return crypto_stream_salsa2012_KEYBYTES;
}

size_t
crypto_stream_salsa2012_noncebytes(void) {
    return crypto_stream_salsa2012_NONCEBYTES;
}

const char *
crypto_stream_salsa2012_primitive(void) {
    return "salsa2012";
}
