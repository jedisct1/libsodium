#include "crypto_stream_salsa20.h"
#include "crypto_stream_xsalsa20.h"

size_t
crypto_stream_xsalsa20_keybytes(void) {
    return crypto_stream_xsalsa20_KEYBYTES;
}

size_t
crypto_stream_xsalsa20_noncebytes(void) {
    return crypto_stream_xsalsa20_NONCEBYTES;
}


int crypto_stream_xsalsa20_xor(unsigned char *c, const unsigned char *m,
                               unsigned long long mlen, const unsigned char *n,
                               const unsigned char *k)
{
    return crypto_stream_salsa20_xor_ic(c, m, mlen, n, 0U, k);
}
