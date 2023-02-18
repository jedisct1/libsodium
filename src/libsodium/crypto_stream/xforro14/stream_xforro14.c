
#include <stdlib.h>

#include "crypto_core_hforro14.h"
#include "crypto_stream_forro14.h"
#include "crypto_stream_xforro14.h"
#include "private/common.h"
#include "randombytes.h"

size_t
crypto_stream_xforro14_keybytes(void)
{
    return crypto_stream_xforro14_KEYBYTES;
}

size_t
crypto_stream_xforro14_noncebytes(void)
{
    return crypto_stream_xforro14_NONCEBYTES;
}

size_t
crypto_stream_xforro14_messagebytes_max(void)
{
    return crypto_stream_xforro14_MESSAGEBYTES_MAX;
}

int crypto_stream_xforro14(unsigned char *c, unsigned long long clen,
                           const unsigned char *n, const unsigned char *k)
{
    unsigned char k2[crypto_core_hforro14_OUTPUTBYTES];

    crypto_core_hforro14(k2, n, k, NULL);
    COMPILER_ASSERT(crypto_stream_forro14_KEYBYTES <= sizeof k2);
    COMPILER_ASSERT(crypto_stream_forro14_NONCEBYTES ==
                    crypto_stream_xforro14_NONCEBYTES -
                        crypto_core_hforro14_INPUTBYTES);

    return crypto_stream_forro14(c, clen, n + crypto_core_hforro14_INPUTBYTES,
                                 k2);
}

int crypto_stream_xforro14_xor_ic(unsigned char *c, const unsigned char *m,
                                  unsigned long long mlen, const unsigned char *n,
                                  uint64_t ic, const unsigned char *k)
{
    unsigned char k2[crypto_core_hforro14_OUTPUTBYTES];

    crypto_core_hforro14(k2, n, k, NULL);
    return crypto_stream_forro14_xor_ic(
        c, m, mlen, n + crypto_core_hforro14_INPUTBYTES, ic, k2);
}

int crypto_stream_xforro14_xor(unsigned char *c, const unsigned char *m,
                               unsigned long long mlen, const unsigned char *n,
                               const unsigned char *k)
{
    return crypto_stream_xforro14_xor_ic(c, m, mlen, n, 0U, k);
}

void crypto_stream_xforro14_keygen(
    unsigned char k[crypto_stream_xforro14_KEYBYTES])
{
    randombytes_buf(k, crypto_stream_xforro14_KEYBYTES);
}
