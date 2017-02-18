#include "crypto_secretbox_xchacha20poly1305.h"

size_t
crypto_secretbox_xchacha20poly1305_keybytes(void)
{
    return crypto_secretbox_xchacha20poly1305_KEYBYTES;
}

size_t
crypto_secretbox_xchacha20poly1305_noncebytes(void)
{
    return crypto_secretbox_xchacha20poly1305_NONCEBYTES;
}

size_t
crypto_secretbox_xchacha20poly1305_macbytes(void)
{
    return crypto_secretbox_xchacha20poly1305_MACBYTES;
}
