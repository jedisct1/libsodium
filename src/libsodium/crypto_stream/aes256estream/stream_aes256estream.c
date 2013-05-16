#include "crypto_stream_aes256estream.h"

size_t
crypto_stream_aes256estream_keybytes(void) {
    return crypto_stream_aes256estream_KEYBYTES;
}

size_t
crypto_stream_aes256estream_noncebytes(void) {
    return crypto_stream_aes256estream_NONCEBYTES;
}

size_t
crypto_stream_aes256estream_beforenmbytes(void) {
    return crypto_stream_aes256estream_BEFORENMBYTES;
}

const char *
crypto_stream_aes256estream_primitive(void) {
    return "aes256estream";
}
