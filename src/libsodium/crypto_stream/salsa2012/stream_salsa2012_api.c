#include "crypto_stream_salsa2012.h"
#include "randombytes.h"

size_t
crypto_stream_salsa2012_keybytes(void)
{
    return crypto_stream_salsa2012_KEYBYTES;
}

size_t
crypto_stream_salsa2012_noncebytes(void)
{
    return crypto_stream_salsa2012_NONCEBYTES;
}

void
crypto_stream_salsa2012_keygen(unsigned char k[crypto_stream_salsa2012_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_salsa2012_KEYBYTES);
}
